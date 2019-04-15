#include <cassert>
#include <cmath>
#include <iostream>

#include "materials.h"
#include "mesh.h"
#include "particle.h"
#include "shapes.h"
#include "simple_structs.h"

int main()
{
    MaterialLibrary materials = C5G7();
    Mesh mesh(10.0f, 10.0f, &materials.get_by_name("Moderator"));

    Color black = {0.0f, 0.0f, 0.0f, 0.0f};
    mesh.add_shape(std::make_unique<Circle>(black, Vec2{0.0f, 0.0f}, 1.0f),
                   &materials.get_by_name("UO2"));

    Circle c1(black, Vec2{0.0f, 0.0f}, 1.0f);
    Particle p1(Vec2{0.0f, 0.0f}, Vec2{std::sin(0.5f), std::cos(0.5f)});
    float d_to_s = c1.distance_to_surface(p1.location, p1.direction, false);
    std::cout << d_to_s << "\n";
    assert(d_to_s == 1.0f);

    return 0;
}