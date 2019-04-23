#pragma once
#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include "GL/gl.h"
#include "GL/glut.h"

struct Ortho2D {
    float left;
    float right;
    float bottom;
    float top;

    Ortho2D() : left(-1.0f), right(1.0f), top(-1.0f), bottom(1.0f)
    {
        return;
    }

    Ortho2D(float left, float right, float top, float bottom)
        : left(left), right(right), top(top), bottom(bottom)
    {
        return;
    }

    void apply() const
    {
        glLoadIdentity();
        gluOrtho2D(left, right, top, bottom);
    }
};