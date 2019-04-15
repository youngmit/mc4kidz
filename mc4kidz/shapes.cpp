#include "shapes.h"

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include "GL/gl.h"
#include "GL/glut.h"

#include <array>
#include <cmath>
#include <limits>
#include <optional>

static const float M_PI = 3.14159265359f;

Circle::Circle(Color c, Vec2 center, float r) : Shape(c), center(center), r(r)
{
    return;
}

// Circle::Circle(Color c, Vec2 center, float r, int segments)

void Circle::draw() const
{
    glBegin(GL_TRIANGLE_FAN);
    glColor4f(color.r, color.g, color.b, color.a);
    for (unsigned int i = 0; i < segments; ++i) {
        float angle = (float)i * 2.0f * M_PI / (float)segments;
        glVertex2f(r * cos(angle) + center.x, r * sin(angle) + center.y);
    }
    glEnd();
}
float Circle::distance_to_surface(Vec2 p, Vec2 dir, bool coincident) const
{
    std::numeric_limits<float> lim;

    std::optional<float> opt = std::nullopt;
    float x                  = p.x - center.x;
    float y                  = p.y - center.y;

    float k   = x * dir.x + y * dir.y;
    float c   = x * x + y * y - r * r;
    float det = k * k - c;

    if (det < 0.0) {
        return lim.max();
    }

    if (coincident) {
        if (k >= 0.0f) {
            return lim.max();
        } else {
            return -k + std::sqrt(det);
        }
    }

    if (c < 0.0f) {
        return -k + std::sqrt(det);
    } else {
        float d = -k - std::sqrt(det);
        return d >= 0.0f ? d : lim.max();
    }
}

void Box::draw() const
{
    glBegin(GL_QUADS);
    glColor4f(color.r, color.g, color.b, color.a);
    glVertex2f(_min_x, _min_y);
    glVertex2f(_max_x, _min_y);
    glVertex2f(_max_x, _max_y);
    glVertex2f(_min_x, _max_y);
    glEnd();

    if (outline_color.has_value()) {
        Color c = outline_color.value();
        glColor4f(c.r, c.g, c.b, c.a);
        glBegin(GL_LINE_LOOP);
        glVertex2f(_min_x, _min_y);
        glVertex2f(_max_x, _min_y);
        glVertex2f(_max_x, _max_y);
        glVertex2f(_min_x, _max_y);
        glEnd();
    }
}

bool Box::point_inside(Vec2 p) const
{
    return p.x > _min_x && p.x < _max_x && p.y > _min_y && p.y < _max_y;
}