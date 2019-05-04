#include "playbook.h"

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "pin_types.h"
#include "state.h"

#undef VOID

// trim from start (in place)
static inline void ltrim(std::string &s)
{
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(), [](int ch) { return !std::isspace(ch); }));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) { return !std::isspace(ch); })
                .base(),
            s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s)
{
    ltrim(s);
    rtrim(s);
}

class NoOpCommand : public Command {
public:
    NoOpCommand()
    {
        std::cout << "wait";
        return;
    };

    void execute(State *state) const override final
    {
        return;
    }
};

class ResetCommand : public Command {
public:
    ResetCommand()
    {
        std::cout << "resetting";
        return;
    };

    void execute(State *state) const override final
    {
        state->reset(false);
    }
};

class ToggleBoundaryCommand : public Command {
public:
    ToggleBoundaryCommand()
    {
        std::cout << "toggling boundary condition";
        return;
    }

    void execute(State *state) const override final
    {
        state->toggle_boundary_condition();
    }
};

class ToggleMaterialCommand : public Command {
public:
    ToggleMaterialCommand(std::stringstream &args)
    {
        args >> _x;
        args >> _y;
        std::cout << "cycling material at (" << _x << ", " << _y << ")";
        return;
    }

    void execute(State *state) const override final
    {
        state->cycle_shape(_x, _y);
    }

private:
    float _x;
    float _y;
};

class CycleAllPinsCommand : public Command {
public:
    CycleAllPinsCommand()
    {
        std::cout << "cycle all pin materials";
        return;
    }

    void execute(State *state) const override final
    {
        state->cycle_all();
    }
};

class SetMaterialCommand : public Command {
public:
    SetMaterialCommand(std::stringstream &args)
    {
        args >> _x;
        args >> _y;
        std::string material;
        args >> material;

        auto material_iter = _material_map.find(material);
        if (material_iter == _material_map.end()) {
            std::stringstream msg;
            msg << "failed to parse material specification: " << material;
            throw msg.str();
        }
        _material = material_iter->second;

        std::cout << "Setting material to " << material << " at (" << _x << ", " << _y
                  << ")";
        return;
    }

    void execute(State *state) const override final
    {
        state->set_material_at(Vec2{_x, _y}, _material);
    }

private:
    float _x;
    float _y;
    PinType _material;
    static const std::unordered_map<std::string, PinType> _material_map;
};

const std::unordered_map<std::string, PinType> SetMaterialCommand::_material_map = {
    {"fuel", PinType::FUEL},
    {"moderator", PinType::MODERATOR},
    {"control", PinType::BLACK},
    {"void", PinType::VOID}};

class AddParticlesCommand : public Command {
public:
    AddParticlesCommand(std::stringstream &args)
    {
        args >> _x;
        args >> _y;
        args >> _n;
        std::cout << "adding " << _n << " particles at (" << _x << ", " << _y << ")";
        return;
    }

    void execute(State *state) const override final
    {
        state->add_particles(Vec2{_x, _y}, _n);
    }

private:
    float _x;
    float _y;
    float _n;
};

Playbook::Playbook()
{
    _commands.push_back(std::make_pair(
        0, std::make_unique<AddParticlesCommand>(std::stringstream("8.5 8.5 200"))));
    return;
}

Playbook::Playbook(const std::string &fname)
{
    std::ifstream in_file(fname);
    if (!in_file.good()) {
        std::stringstream msg;
        msg << "Failed to open playbook file: " << fname;
        auto str = msg.str();
        throw str;
    }

    while (in_file.good()) {
        std::string line_src;
        std::getline(in_file, line_src);
        trim(line_src);

        if (line_src.size() == 0) {
            continue;
        }

        if (line_src[0] == '#') {
            continue;
        }

        std::stringstream line(line_src);

        // Parse the number of tics until the following instruction
        size_t n_tics;
        line >> n_tics;

        // Try to parse the instruction
        std::string instruction_str;
        line >> instruction_str;

        auto inst_iter = _instruction_map.find(instruction_str);
        if (inst_iter == _instruction_map.end()) {
            std::stringstream msg;
            msg << "Failed to parse instruction: " << instruction_str;
            throw msg.str();
        }
        Instruction instruction = inst_iter->second;

        // Based on the instruction, parse arguments or not
        std::unique_ptr<Command> cmd = nullptr;

        std::cout << "After " << n_tics << " tics, ";
        switch (instruction) {
        case Instruction::RESET:
            cmd = std::make_unique<ResetCommand>();
            break;

        case Instruction::TOGGLE_BOUNDARY:
            cmd = std::make_unique<ToggleBoundaryCommand>();
            break;

        case Instruction::TOGGLE_MATERIAL:
            cmd = std::make_unique<ToggleMaterialCommand>(line);
            break;
        case Instruction::CYCLE_ALL:
            cmd = std::make_unique<CycleAllPinsCommand>();
            break;
        case Instruction::SET_MATERIAL:
            cmd = std::make_unique<SetMaterialCommand>(line);
            break;
        case Instruction::ADD_PARTICLES:
            cmd = std::make_unique<AddParticlesCommand>(line);
            break;
        case Instruction::NOOP:
            cmd = std::make_unique<NoOpCommand>();
            break;
        case Instruction::HALT:
            // First halt instruction encountered stops the playbook
            if (!_stop_command) {
                _stop_command = _commands.size();
            }
            // insert a noop so that there is a slot for this instruction
            cmd = std::make_unique<NoOpCommand>();
            break;
        }
        std::cout << "\n";

        if (cmd) {
            _commands.push_back(std::make_pair(n_tics, std::move(cmd)));
        }
    }
}

const std::unordered_map<std::string, Instruction> Playbook::_instruction_map = {
    {"reset", Instruction::RESET},
    {"toggle_boundary", Instruction::TOGGLE_BOUNDARY},
    {"toggle_material", Instruction::TOGGLE_MATERIAL},
    {"cycle_all", Instruction::CYCLE_ALL},
    {"set_material", Instruction::SET_MATERIAL},
    {"add_particles", Instruction::ADD_PARTICLES},
    {"noop", Instruction::NOOP},
    {"halt", Instruction::HALT}};
