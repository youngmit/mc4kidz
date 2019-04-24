#include "line_plot.h"

#include <vector>
#include <numeric>

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
    size_t domain    = data.capacity();
    size_t n_points  = data.size();

    float dx = 2.0f / domain;
    auto max_pop = *std::max_element(data.begin(), data.end());
    float scale  = 2.0f / max_pop;

    glBegin(GL_LINE_STRIP);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    int i = 0;
    for (auto val : data) {
        glVertex2f(dx*i - 1.0f, scale*val - 1.0f);
        ++i;
    }
    glEnd();
    return;
}
