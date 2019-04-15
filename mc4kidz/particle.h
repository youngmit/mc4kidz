#pragma once
#include <cmath>
#include <iostream>
#include <random>

#include "simple_structs.h"
#include "materials.h"

extern std::uniform_real_distribution<float> uniform;

struct Particle {
public:
    Particle(Vec2 l, Vec2 v) : location(l), direction(v), alive(true)
    {
        return;
    }

    bool tic(float t)
    {
        Vec2 delta = direction * speed * t;
        distance -= delta.norm();
        if (distance >= 0.0f) {
            location += delta;
        }

        return distance < 0.0;
    }

    float sample_distance(std::default_random_engine &r)
    {
        assert(material);
        distance = -std::log(uniform(r)) / material->xstr[e_group];
        return distance;
    }

    Vec2 location;
    Vec2 direction;
    std::vector<Vec2> waypoints;
    float speed    = 0.05f;
    float distance = 1.0f;
    int e_group    = 0;
    bool alive     = true;

    const Material *material = nullptr;
};
