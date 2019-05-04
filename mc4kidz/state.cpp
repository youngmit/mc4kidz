#include "state.h"

#include <memory>
#include <sstream>
#include <tuple>
#include <unordered_map>

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include "GL/freeglut.h"
#include "GL/gl.h"

#include "materials.h"
#include "pin_types.h"
#include "shapes.h"

#undef VOID

std::vector<Color> make_particle_colors()
{
    std::vector<Color> colors;

    colors.push_back(Color{1.0f, 0.0f, 0.0f, 1.0f});   // Red
    colors.push_back(Color{1.0f, 0.647f, 0.0f, 1.0f}); // Orange
    colors.push_back(Color{1.0f, 1.0f, 0.0f, 1.0f});   // Yellow
    colors.push_back(Color{0.0f, 1.0f, 0.0f, 1.0f});   // Green
    colors.push_back(Color{0.0f, 0.0f, 1.0f, 1.0f});   // Blue
    colors.push_back(Color{0.901f, 0.0f, 1.0f, 1.0f}); // purple

    return colors;
}

State::State()
    : _particle_colors(make_particle_colors()),
      _materials(C5G7()),
      _mesh(NPINS_X * PIN_PITCH, NPINS_Y * PIN_PITCH,
            &_materials.get_by_name("Moderator"), MODERATOR_COLOR),
      _boundary(Color{0.0f, 0.0f, 0.0f, 0.0f}, Vec2{0.0f, 0.0f},
                Vec2{_mesh.get_width(), _mesh.get_height()}),
      _angle_distribution(0, 6.28318530718f)
{
    Color gray{0.3f, 0.3f, 0.3f, 1.0f};
    Color fuel{0.4f, 0.0f, 0.0f, 1.0f};
    Color white{1.0f, 1.0f, 1.0f, 1.0f};
    Color black{0.0f, 0.0f, 0.0f, 1.0f};

    _pin_types[PinType::FUEL] = std::make_tuple(fuel, &_materials.get_by_name("UO2"));
    _pin_types[PinType::MODERATOR] =
        std::make_tuple(MODERATOR_COLOR, &_materials.get_by_name("Moderator"));
    _pin_types[PinType::BLACK] =
        std::make_tuple(gray, &_materials.get_by_name("Control"));
    _pin_types[PinType::VOID] = std::make_tuple(black, &_materials.get_by_name("Void"));

    _boundary.outline_color = white;

    for (int ix = 0; ix < NPINS_X; ++ix) {
        for (int iy = 0; iy < NPINS_Y; ++iy) {
            const Material *mat = (ix == iy) ? &_materials.get_by_name("UO2")
                                             : &_materials.get_by_name("UO2");
            auto color = ix == iy ? fuel : fuel;
            _mesh.add_shape(
                std::make_unique<Circle>(color, Vec2{0.5f * PIN_PITCH + ix * PIN_PITCH,
                                                     0.5f * PIN_PITCH + iy * PIN_PITCH},
                                         PIN_RADIUS),
                mat);
        }
    }

    // Some reasonable amount of particles to start
    _particles.reserve(1'500);

    reset();

    return;
}

void State::reset(bool hard)
{
    _particles.clear();
    _process_queue.clear();
    _generation_born.clear();
    _generation_population.clear();
    _population_history.clear();
    _population_history.reserve(MAX_POP_HIST);
    _history_resolution = 1;
    _time_step          = 0;

    _n_capture = 0;
    _n_fission = 0;
    _n_scatter = 0;
    _n_leak    = 0;

    _generation_born.push_back(0);
    _generation_population.push_back(0);

    if (hard) {
        _since_last_command = 0;

        if (_playbook) {
            _playbook->reset();
            _next_command = _playbook->tics_to_next();
        } else {
            _next_command = std::numeric_limits<size_t>::max();
        }
    }
}

void State::tic(bool force)
{
    if (_paused ^ force) {
        return;
    }

    if (_since_last_command == _next_command) {
        _since_last_command = 0;
        if (_playbook) {
            _next_command = _playbook->execute_next(this);
        } else {
            _next_command = std::numeric_limits<size_t>::max();
        }
    }

    _time_step++;
    _since_last_command++;

    if (_source) {
        Vec2 location = _source.value();

        Particle p = _new_particle(location);
        _generation_born[0]++;
        _generation_population[0]++;
        _mesh.transport_particle(p, _random);
        _particles.push_back(p);
    }

    _process_queue.clear();
    size_t id = 0;
    for (auto &p : _particles) {
        bool process = p.tic(1.0f);

        // Handle the boundary condition
        if (p.location.x < 0.0f || p.location.x > _mesh.get_width() ||
            p.location.y < 0.0f || p.location.y > _mesh.get_height()) {
            if (_bc == BoundaryCondition::VACUUM) {
                p.alive = false;
                process = false;
                _n_leak++;
                _generation_population[p.generation]--;
                continue;
            }
            if (_bc == BoundaryCondition::REFLECTIVE) {
                if (p.location.x < 0.0f) {
                    p.direction.x = -p.direction.x;
                    p.location.x  = 0.0f;
                }
                if (p.location.x > _mesh.get_width()) {
                    p.direction.x = -p.direction.x;
                    p.location.x  = _mesh.get_width();
                }
                if (p.location.y < 0.0f) {
                    p.direction.y = -p.direction.y;
                    p.location.y  = 0.0f;
                }
                if (p.location.y > _mesh.get_height()) {
                    p.direction.y = -p.direction.y;
                    p.location.y  = _mesh.get_height();
                }
                _mesh.transport_particle(p, _random);
            }
        }

        if (process) {
            _process_queue.push_back(id);
        }

        ++id;
    }

    for (const auto id : _process_queue) {
        interact(id);
    }

    _process_queue.clear();

    // remove dead particles
    auto new_end = std::remove_if(_particles.begin(), _particles.end(),
                                  [](Particle &p) { return !p.alive; });
    _particles.erase(new_end, _particles.end());

    if (_time_step % _history_resolution == 0) {
        if (_population_history.size() == MAX_POP_HIST) {
            _resample_population();
        }
        _population_history.push_back(_particles.size());
    }
    assert(_particles.size() == std::accumulate(_generation_population.begin(),
                                                _generation_population.end(), 0));

    if (_time_step > 10000) {
        toggle_pause();
    }
}

void State::resample()
{
    for (auto &p : _particles) {
        _mesh.transport_particle(p, _random);
    }
}

void State::draw() const
{
    glPushMatrix();

    _projection_matrix.apply();

    _mesh.draw();

    if (_bc == BoundaryCondition::REFLECTIVE) {
        _boundary.draw();
    }

    for (auto &p : _particles) {
        Circle c(_particle_colors[p.generation % _particle_colors.size()], p.location,
                 0.02f);
        c.draw();
        if (_draw_waypoints) {
            for (const auto waypoint : p.waypoints) {
                c = Circle(PARTICLE_DEST_COLOR, waypoint, 0.02f);
                c.draw();
            }
        }
    }

    if (_labels) {
        glColor3f(1.0f, 1.0f, 1.0f);
        for (size_t id = 0; id < _particles.size(); ++id) {
            glRasterPos2f(_particles[id].location.x, _particles[id].location.y);
            std::stringstream sstream;
            sstream << id;
            auto id_str = sstream.str();
            glutBitmapString(GLUT_BITMAP_HELVETICA_18,
                             (const unsigned char *)id_str.c_str());
        }
    }

    // Print statistics, etc
    std::stringstream sstream;
    sstream << "Neutron population: " << _particles.size();
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(1.0f, 1.0f);
    auto pop_str  = sstream.str();
    const char *c = pop_str.c_str();
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)c);

    sstream.str("");

    sstream << "Mean dist. to collision: " << _mesh.mean_distance_to_collision();
    auto mdtc_str = sstream.str();
    glRasterPos2f(1.0f, 1.5f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)mdtc_str.c_str());

    sstream.str("");

    sstream << "Time step: " << _time_step;
    auto ts_str = sstream.str();
    glRasterPos2f(1.0f, 2.0f);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char *)ts_str.c_str());

    for (size_t gen = 0; gen < _generation_born.size(); ++gen) {
        sstream.str("");
        sstream << "Generation " << gen << ": " << _generation_born[gen] << " ("
                << _generation_population[gen] << ")";
        pop_str = sstream.str();
        glRasterPos2f(_mesh.get_width() + 0.2f, _mesh.get_height() - 0.5f - 0.5f * gen);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18,
                         (const unsigned char *)pop_str.c_str());
    }

    glPopMatrix();
}

void State::interact(size_t id)
{
    Vec2 test_center        = {5.5f, 5.5f};
    float r                 = _unit_distribution(_random);
    Particle &p             = _particles[id];
    Material const *mat     = p.material;
    Interaction interaction = mat->interaction_cdf[p.e_group].sample(r);

    if (interaction == Interaction::CAPTURE) {
        _n_capture++;
        p.alive = false;
        _generation_population[p.generation] -= 1;
        return;
    }
    if (interaction == Interaction::SCATTER) {
        _n_scatter++;
        float angle  = _angle_distribution(_random);
        float scat_r = _unit_distribution(_random);
        p.e_group    = mat->scatter_cdf[p.e_group].sample(scat_r);
        p.direction  = {std::sin(angle), std::cos(angle)};
        _mesh.transport_particle(p, _random);
        return;
    }
    if (interaction == Interaction::FISSION) {
        _n_fission++;
        p.alive = false;
        _generation_population[p.generation] -= 1;
        Particle old_p = p;
        float new_r    = _unit_distribution(_random);
        int nu         = new_r > 0.5 ? 3 : 2;
        for (int i = 0; i < nu; ++i) {
            Particle p2 = _new_particle(old_p.location);
            // TODO: Actually sample chi distribution
            p2.e_group    = 0;
            p2.generation = old_p.generation + 1;
            if (p2.generation > _generation_born.size() - 1) {
                _generation_born.push_back(0);
                _generation_population.push_back(0);
            }
            _generation_born[p2.generation] += 1;
            _generation_population[p2.generation] += 1;
            p2.material = mat;
            _mesh.transport_particle(p2, _random);
            _particles.push_back(p2);
        }
        return;
    }
    assert(false);
    return;
}

// Double the history resolution and discard every other population sample.
// Shift remaining values down and resize the vector
void State::_resample_population()
{
    _history_resolution *= 2;
    for (size_t i = 0; i < _population_history.size() / 2; ++i) {
        _population_history[i] = _population_history[i * 2];
    }

    _population_history.resize(_population_history.size() / 2);
    return;
}

void State::toggle_boundary_condition()
{
    switch (_bc) {
    case BoundaryCondition::VACUUM:
        _bc = BoundaryCondition::REFLECTIVE;
        break;
    case BoundaryCondition::REFLECTIVE:
        _bc = BoundaryCondition::VACUUM;
        break;
    }
}

void State::set_material_at(Vec2 location, PinType material)
{
    auto[new_c, new_mat] = _pin_types[material];

    _mesh.set_color_material_at(location, new_c, new_mat);
}

void State::cycle_shape(float x, float y)
{
    Vec2 location = {x, y};
    auto result   = _mesh.get_color_material_at(location);
    if (!result) {
        return;
    }

    auto[color, mat] = result.value();
    PinType new_type;
    if (mat == &_materials.get_by_name("Moderator")) {
        new_type = PinType::FUEL;
    } else if (mat == &_materials.get_by_name("UO2")) {
        new_type = PinType::BLACK;
    } else if (mat == &_materials.get_by_name("Control")) {
        new_type = PinType::VOID;
    } else if (mat == &_materials.get_by_name("Void")) {
        new_type = PinType::MODERATOR;
    }
    auto[new_c, new_mat] = _pin_types[new_type];
    _mesh.set_color_material_at(location, new_c, new_mat);

    resample();

    return;
}

void State::cycle_all()
{
    PinType new_type;
    if (_current_pin_type == PinType::MODERATOR) {
        new_type = PinType::FUEL;
    } else if (_current_pin_type == PinType::FUEL) {
        new_type = PinType::BLACK;
    } else if (_current_pin_type == PinType::BLACK) {
        new_type = PinType::VOID;
    } else if (_current_pin_type == PinType::VOID) {
        new_type = PinType::MODERATOR;
    }
    auto[new_c, new_mat] = _pin_types[new_type];
    _mesh.set_color_material_all_shapes(new_c, new_mat);
    _current_pin_type = new_type;

	resample();
}

Particle State::_new_particle(Vec2 location) const
{
    float angle = _angle_distribution(_random);
    Vec2 direction{std::sin(angle), std::cos(angle)};
    Particle p(location, direction);
    p.material = _mesh.get_material(p.location);
    p.e_group  = 6;

    return p;
}
