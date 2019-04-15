#include "mesh.h"

void Mesh::transport_particle(Particle &particle,
                              std::default_random_engine &random) const
{
    const Material *mat         = particle.material;
    float distance              = 0.0;
    Vec2 location               = particle.location;
    bool coincident             = false;
    size_t coincident_surf      = _shapes.size();
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
        coincident_surf = surface_i;


        if (d_to_c < d_to_s) {
            // Particle didn't make it to the surface. No need to update material
            distance += d_to_c;
            break;
        } else {
            // Particle made it into the next region. Update the location to a
            // little past the surface to prevent weirdness. This is usually
            // more difficult, since we dont allow shapes to intersect.
            auto delta = particle.direction * (d_to_s * 1.00001f);
            location   = location + delta;
            particle.waypoints.push_back(location);
            distance += d_to_s;
            // coincident = true;

            // See if we are inside or outside of the shape; that should be all
            // we need to know.
            if (surface_i < _shapes.size() &&
                _shapes[surface_i]->point_inside(location)) {
                mat = _materials[surface_i];
            } else {
                mat = _inter_mat;
            }
            particle.material = mat;
        }
    }

	_total_distance += distance;
    _n_collisions++;

    particle.distance = distance;
    particle.waypoints.push_back(particle.location + particle.direction * distance);
}