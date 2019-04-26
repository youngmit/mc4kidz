#include "line_plot.h"

#include <numeric>
#include <vector>

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include "GL/freeglut.h"
#include "GL/gl.h"

void LinePlot::draw() const
{
    const auto &data = get_data();
    if (data.size() < 1) {
        return;
    }
    size_t domain   = data.capacity();
    size_t n_points = data.size();

    float dx     = 2.0f / domain;
    auto max_pop = *std::max_element(data.begin(), data.end());
    float scale  = 2.0f / max_pop;

    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-1.01f, 1.01f, -1.01f, 1.01f);

    glBegin(GL_LINES);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(1.0f, -1.0f);
    glVertex2f(-1.0f, -1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();

    glBegin(GL_LINE_STRIP);
    int i = 0;
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    for (size_t i = 0; i < data.size(); ++i) {
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glVertex2f(dx * i - 1.0f, scale * data[i] - 1.0f);
    }
    glEnd();

    glPopMatrix();
    return;
}
