#pragma once
#include <cstdint>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class State;

enum class Instruction : uint8_t {
    RESET,
    TOGGLE_BOUNDARY,
    TOGGLE_MATERIAL,
	CYCLE_ALL,
	SET_MATERIAL,
	ADD_PARTICLES,
    NOOP,
    HALT
};

class Command {
public:
    virtual void execute(State *state) const = 0;
};

class Playbook {
public:
    Playbook();
    Playbook(const std::string &fname);

    // Execute commands until a circuit is completed or we need to do more
    // simulation to get to the next command. Return the number of tics to get to
    // the next command.
    size_t execute_next(State *state)
    {
        if (_commands.size() == 0) {
            return std::numeric_limits<size_t>::max();
        }

        size_t current_command = _next_command;
        while (true) {
            _commands[_next_command].second->execute(state);
            _next_command = (_next_command + 1) % _commands.size();

            // Avoid infinite loops, if all commands happen after zero time
            if (_next_command == current_command) {
                break;
            }

            // Execute commands until the next has a non-zero time
            if (_commands[_next_command].first > 0) {
                break;
            }

			// Tell the caller to go on forever if we get to the halt command
            if (_stop_command && _next_command == _stop_command.value()) {
                return std::numeric_limits<size_t>::max();
            }
        }

        return _commands[_next_command].first;
    }

    size_t tics_to_next() const
    {
        if (_commands.size() == 0) {
            return std::numeric_limits<size_t>::max();
        }
        return _commands[_next_command].first;
    }

    void reset()
    {
        _next_command = 0;
    }

private:
    size_t _next_command                = 0;
    std::optional<size_t> _stop_command = std::nullopt;
    std::vector<std::pair<size_t, std::unique_ptr<Command>>> _commands;

    static const std::unordered_map<std::string, Instruction> _instruction_map;
};