#pragma once
#include <cassert>
#include <cmath>
#include <cstdint>
#include <random>
#include <vector>
#include <tuple>
#include <unordered_map>

#include "materials.h"
#include "mesh.h"
#include "particle.h"
#include "shapes.h"

enum class BoundaryCondition : uint8_t { VACUUM, REFLECTIVE };

enum class PinType : uint8_t {FUEL, MODERATOR, BLACK};

class State {
public:
    State();

    // Wipe out any existing particles and spawn a new batch
    void reset();

	// Re-transport all particles; we do this when the system changes
	void resample();

    // Simulate the motion of particles for a frame/time step.
    // Handle collisions that occur
    void tic(bool force=false);

    void draw() const;

    // Switch to the next boundary condition type
    void toggle_boundary_condition();

    void toggle_waypoints()
    {
        _draw_waypoints = !_draw_waypoints;
    }

    void toggle_pause(){
        _paused = !_paused;
        std::cout << "Pause: " << _paused << "\n";
    }

    void toggle_labels() {
        _labels = !_labels;
    }

	// Cycle the shape in the mesh from one to the next in a collection
    void cycle_shape(float x, float y);

    // Sample a particle interaction. Return whether the particle survived.
    void interact(size_t id);

private:
    const Color PARTICLE_COLOR{1.0f, 1.0f, 0.0f, 1.0f};
    const Color PARTICLE_DEST_COLOR{0.0f, 0.0f, 1.0f, 1.0f};
    const Color PIN_COLOR{0.5f, 0.0f, 0.0f, 1.0f};
    const int NPINS_X      = 10;
    const int NPINS_Y      = 10;
    const float PIN_RADIUS = 0.4f;
    const float PIN_PITCH  = 1.0f;

    std::vector<Particle> _particles;
    // Collection of indices into _particles which need to process a collision
    // this time step.
    // Kept as object state to prevent reallocation.
    std::vector<size_t> _process_queue;

    // Total number of particles born into each generation
    std::vector<unsigned int> _generation_population;
    
    MaterialLibrary _materials;
    Mesh _mesh;
    Box _boundary;
    BoundaryCondition _bc = BoundaryCondition::VACUUM;

    // RNG stuff
    std::default_random_engine _random;
    std::uniform_real_distribution<float> _angle_distribution;
    std::uniform_real_distribution<float> _unit_distribution;

    // Drawing settings
    bool _draw_waypoints = false;
    bool _paused = true;
    bool _labels = false;

	std::unordered_map<PinType, std::tuple<Color, const Material *>> _pin_types;
};
