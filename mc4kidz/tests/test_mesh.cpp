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

    // Test some vector stuff
    Vec2 v1{1.0, 1.0};
    Vec2 v2{1.5, 2.0};
    Vec2 v3{0.5, 1.0};
    assert(v2 - v1 == v3);

    assert(std::abs(v2.norm() - 2.5) < 1e-6);

    v1 = {5.23820633f, 5.15463161f};
    v2 = {5.5, 5.5};
    v3 = v1 - v2;
    float norm = v3.norm();
    assert(std::abs((v1 - v2).norm() - 0.4333766f) < 1.0e-5);

    return 0;
}