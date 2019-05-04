#pragma once
#include <cassert>
#include <cmath>
#include <cstdint>
#include <optional>
#include <random>
#include <tuple>
#include <unordered_map>
#include <vector>

#include "materials.h"
#include "mesh.h"
#include "particle.h"
#include "pin_types.h"
#include "playbook.h"
#include "shapes.h"
#include "view.h"

enum class BoundaryCondition : uint8_t { VACUUM, REFLECTIVE };


class State {
public:
    State();

    // Wipe out any existing particles and spawn a new batch.
    // A "soft" reset is performed by automated controls, and doesnt reset the parts of
    // the
    // state that track progress through a Playbook. A "hard" reset happens at the very
    // beginning to get things going and by the user.
    void reset(bool hard = true);

    // Re-transport all particles; we do this when the system changes
    void resample();

    // Simulate the motion of particles for a frame/time step.
    // Handle collisions that occur
    void tic(bool force = false);

    void draw() const;

    void add_particles(Vec2 location, int n)
    {
        for (int i = 0; i < n; ++i) {
            Particle p = _new_particle(location);
            _mesh.transport_particle(p, _random);
            _particles.push_back(p);
            _generation_born[0]++;
            _generation_population[0]++;
        }
    }

    // Associate a playbook to automate controls
    void set_playbook(std::unique_ptr<Playbook> playbook)
    {
        _playbook = std::move(playbook);
        reset();
    }

    // Switch to the next boundary condition type
    void toggle_boundary_condition();

    void toggle_waypoints()
    {
        _draw_waypoints = !_draw_waypoints;
    }

    void toggle_pause()
    {
        _paused = !_paused;
        std::cout << "Pause: " << _paused << "\n";
    }

    void toggle_labels()
    {
        _labels = !_labels;
    }

    void set_source(float x, float y)
    {
        _source = Vec2{x, y};
    }

    void unset_source()
    {
        _source = std::nullopt;
    }

    bool has_source() const
    {
        return _source.has_value();
    }

    void set_view(Ortho2D view)
    {
        _projection_matrix = view;
    }

	void set_material_at(Vec2 location, PinType material);

    // Cycle the shape in the mesh from one to the next in a collection
    void cycle_shape(float x, float y);

    void cycle_all();

    // Sample a particle interaction. Return whether the particle survived.
    void interact(size_t id);

    std::vector<unsigned int> get_interaction_counts() const
    {
        return {_n_scatter, _n_capture, _n_fission, _n_leak};
    }

    std::vector<unsigned int> get_spectrum() const
    {
        std::vector<unsigned int> spectrum(7, 0);
        for (const auto &p : _particles) {
            spectrum[p.e_group]++;
        }
        return spectrum;
    }

    const std::vector<unsigned int> &get_population_history() const
    {
        return _population_history;
    }

private:
    Particle _new_particle(Vec2 location) const;

    const Color PARTICLE_DEST_COLOR{0.0f, 0.0f, 1.0f, 1.0f};
    const Color PIN_COLOR{0.3f, 0.0f, 0.0f, 1.0f};
    const Color MODERATOR_COLOR{0.0f, 0.1f, 0.3f, 1.0f};
    const int NPINS_X         = 17;
    const int NPINS_Y         = 17;
    const float PIN_RADIUS    = 0.4f;
    const float PIN_PITCH     = 1.0f;
    const size_t MAX_POP_HIST = 1000;

    const std::vector<Color> _particle_colors;

	// Current set of particles
    std::vector<Particle> _particles;

	// "Back" set of particles. Fission neutrons are born in here, surviving particles stashed here as well. swap at end of tic()
    std::vector<Particle> _back_particles;

	// Collection of indices into _particles which need to process a collision
    // this time step.
    // Kept as object state to prevent reallocation.
    std::vector<size_t> _process_queue;

    // Total number of particles born into each generation
    std::vector<unsigned int> _generation_born;
    // Total number of particles remaining in each generation
    std::vector<unsigned int> _generation_population;
    // Population history by time step. Only store a certain number of time steps
    std::vector<unsigned int> _population_history;
    // Log history every n time steps. This will grow to keep the number of history
    // values bounded
    unsigned int _history_resolution = 1;
    unsigned int _time_step          = 0;
    size_t _since_last_command       = 0;
    size_t _next_command             = 0;

    MaterialLibrary _materials;
    Mesh _mesh;
    Box _boundary;
    BoundaryCondition _bc = BoundaryCondition::VACUUM;

    // RNG stuff
    mutable std::default_random_engine _random;
    mutable std::uniform_real_distribution<float> _angle_distribution;
    std::uniform_real_distribution<float> _unit_distribution;

    // Drawing settings
    Ortho2D projection_matrix;
    bool _draw_waypoints = false;
    bool _paused         = true;
    bool _labels         = false;

    // Optional playbook
    std::unique_ptr<Playbook> _playbook;

    std::unordered_map<PinType, std::tuple<Color, const Material *>> _pin_types;

    std::optional<Vec2> _source = std::nullopt;

    Ortho2D _projection_matrix;

    // Interaction histories
    unsigned int _n_capture = 0;
    unsigned int _n_fission = 0;
    unsigned int _n_scatter = 0;
    unsigned int _n_leak    = 0;

    PinType _current_pin_type = PinType::FUEL;

    void _resample_population();
};
