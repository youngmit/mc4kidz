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
    Mesh(float width, float height, const Material *inter_mat)
        : _width(width), _height(height), _inter_mat(inter_mat)
    {
        return;
    }

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
            return std::nullopt;
        }
        std::cout << i_reg.value() << "\n";
        return std::make_tuple(_shapes[i_reg.value()]->color,
                               _materials[i_reg.value()]);
    }

    void set_color_material_at(Vec2 location, Color c, const Material *mat)
    {
        auto i_reg = find_region(location);
        if (i_reg) {
            _shapes[i_reg.value()]->color = c;
            _materials[i_reg.value()]     = mat;
        }
        return;
    }

    void draw() const
    {
        for (const auto &s : _shapes) {
            s->draw();
        }
    }

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

private:
    float _width;
    float _height;
    std::vector<std::unique_ptr<Shape>> _shapes;
    std::vector<const Material *> _materials;
    const Material *_inter_mat;
    mutable int _n_collisions     = 0;
    mutable float _total_distance = 0.0f;
};
