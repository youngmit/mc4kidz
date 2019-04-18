#pragma once
#include <cmath>
#include <iostream>
#include <random>

#include "materials.h"
#include "simple_structs.h"

extern std::uniform_real_distribution<float> uniform;

struct Particle {
public:
    Particle(Vec2 l, Vec2 v)
        : location(l), direction(v), alive(true)
    {
        return;
    }

    bool tic(float t)
    {
        float delta = base_speed/(float)(e_group+1) * t;
        bool done  = false;
        if (delta > distance) {
            delta = distance;
            done  = true;
		}
		
        distance -= delta;
        location += direction * delta;

        return done;
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
    static float base_speed;
    float distance = 1.0f;
    int e_group    = 0;
    unsigned int generation = 0;
    bool alive     = true;

    const Material *material = nullptr;
};
