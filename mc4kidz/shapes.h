#pragma once
#include <array>
#include <optional>

#include "simple_structs.h"

class Shape {
public:
    Shape(Color c) : color(c)
    {
    }
    virtual void draw() const = 0;
    virtual float distance_to_surface(Vec2 p, Vec2 dir, bool coincident) const = 0;
    virtual bool point_inside(Vec2 p) const = 0;
    // virtual std::array<std::optional<Vec2>, 2> line_intersect(Vec2 p1, Vec2 p2)
    // const=0;
    Color color;
    std::optional<Color> outline_color = std::nullopt;
};

class Circle : public Shape {
public:
    Circle(Color c, Vec2 center, float r);
    void draw() const;

    float distance_to_surface(Vec2 p, Vec2 dir, bool coincident) const;
    // virtual std::array<std::optional<Vec2>, 2> line_intersect(Vec2 p1, Vec2 p2)
    // const=0;

    bool point_inside(Vec2 p) const
    {
        return (p.x - center.x) * (p.x - center.x) +
                   (p.y - center.y) * (p.y - center.y) <
               r * r;
    }

    Vec2 center;
    float r;
    unsigned int segments = 20;
};

class Box : public Shape {
public:
    Box(Color c, Vec2 p1, Vec2 p2) : Shape(c)
    {
        _min_x = std::min(p1.x, p2.x);
        _min_y = std::min(p1.y, p2.y);
        _max_x = std::max(p1.x, p2.x);
        _max_y = std::max(p1.y, p2.y);
        return;
    }

    bool point_inside(Vec2 p) const;

    float distance_to_surface(Vec2 p, Vec2 dir, bool coincident) const
    {
        return std::numeric_limits<float>::max();
    }

    void draw() const;

private:
    float _min_x;
    float _min_y;
    float _max_x;
    float _max_y;
};