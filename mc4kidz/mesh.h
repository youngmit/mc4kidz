#pragma once
#include <algorithm>
#include <limits>
#include <memory>
#include <optional>
#include <random>
#include <tuple>
#include <vector>

#include "materials.h"
#include "particle.h"
#include "shapes.h"
#include "simple_structs.h"

class Mesh {
public:
    Mesh(float width, float height, const Material *inter_mat, Color background_color);

    void add_shape(std::unique_ptr<Shape> shape, const Material *material)
    {
        _shapes.emplace_back(std::move(shape));
        _materials.push_back(material);
    }

    std::optional<std::tuple<Color, const Material *>>
    get_color_material_at(Vec2 location) const
    {
        auto i_reg = find_region(location);
        if (!i_reg) {
            return std::make_tuple(_background, _inter_mat);
        }
        return std::make_tuple(_shapes[i_reg.value()]->color,
                               _materials[i_reg.value()]);
    }

    void set_color_material_at(Vec2 location, Color c, const Material *mat)
    {
        auto i_reg = find_region(location);
        if (i_reg) {
            _shapes[i_reg.value()]->color = c;
            _materials[i_reg.value()]     = mat;
        } else {
            _inter_mat  = mat;
            _background = c;
        }
        return;
    }

    void set_color_material_all_shapes(Color c, const Material *mat)
    {
        for (auto &shape : _shapes) {
            shape->color = c;
        }
        for (auto &material : _materials) {
            material = mat;
        }
    }

    void draw() const;

    // Simulate the passage of a particle through the mesh to its next interaction.
    // This will:
    //   - Set the particle's distance to travel to it's next interaction site
    //   - Set the particle's material pointer to the material at that site
    void transport_particle(Particle &particle,
                            std::default_random_engine &random) const;

    const Material *get_material(Vec2 location) const
    {
        for (size_t i = 0; i < _shapes.size(); ++i) {
            if (_shapes[i]->point_inside(location)) {
                return _materials[i];
            }
        }
        return _inter_mat;
    }

    float mean_distance_to_collision() const
    {
        return _total_distance / _n_collisions;
    }

    std::optional<size_t> find_region(Vec2 location) const;

    float get_width() const
    {
        return _width;
    }

    float get_height() const
    {
        return _height;
    }

private:
    float _width;
    float _height;
    std::vector<std::unique_ptr<Shape>> _shapes;
    std::vector<const Material *> _materials;
    const Material *_inter_mat;
    Color _background;
    mutable int _n_collisions     = 0;
    mutable float _total_distance = 0.0f;
};
