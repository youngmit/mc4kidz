#include "mesh.h"

Mesh::Mesh(float width, float height, const Material *inter_mat)
    : _width(width), _height(height), _inter_mat(inter_mat)
{
    return;
}

void Mesh::transport_particle(Particle &particle,
                              std::default_random_engine &random) const
{
    auto i_reg             = find_region(particle.location);
    const Material *mat    = particle.material;
    float distance         = 0.0;
    Vec2 location          = particle.location;
    size_t coincident_surf = _shapes.size();

    while (location.x >= 0.0f && location.y >= 0.0f && location.x <= _width &&
           location.y <= _height) {
        // Distance to collision in current material
        float d_to_c = particle.sample_distance(random);
        // Distance to next surface; start with something stupid big
        float d_to_s = std::numeric_limits<float>::max();

        size_t surface_i = _shapes.size();
        for (size_t i = 0; i < _shapes.size(); ++i) {
            float d = _shapes[i]->distance_to_surface(location, particle.direction,
                                                      coincident_surf == i);
            if (d < d_to_s) {
                d_to_s    = d;
                surface_i = i;
            }
        }

        if (d_to_c < d_to_s) {
            // Particle didn't make it to the surface. No need to update material
            distance += d_to_c;
            break;
        } else {
            // Particle made it into the next region. Update the location to a
            // little past the surface to prevent weirdness. This is usually
            // more difficult, since we dont allow shapes to intersect.

            // TODO: I don't think this extra bump should be necessary, but it
            // is. Should circle back to this if i have time...
            auto delta = particle.direction * (d_to_s * 1.0001f);
            location   = location + delta;
            particle.waypoints.push_back(location);
            distance += d_to_s;

            // We know we are crossing a surface, so we are either entering the
            // coincident shape or exiting.
            if (!i_reg) {
                // We __were__ ouside of the surface. now inside
                mat             = _materials[surface_i];
                i_reg           = surface_i;
                coincident_surf = surface_i;
            } else {
                // We __were__ inside the surface. now outside
                mat   = _inter_mat;
                i_reg = std::nullopt;
            }
            particle.material = mat;
        }
    }

    _total_distance += distance;
    _n_collisions++;

    particle.distance = distance;
    particle.waypoints.push_back(particle.location + particle.direction * distance);
}

std::optional<size_t> Mesh::find_region(Vec2 location) const
{
    for (size_t i = 0; i < _shapes.size(); ++i) {
        if (_shapes[i]->point_inside(location)) {
            return i;
        }
    }
    return std::nullopt;
}
