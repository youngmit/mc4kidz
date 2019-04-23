#include "histogram.h"

#include <algorithm>
#include <vector>

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include "GL/freeglut.h"
#include "GL/gl.h"

void Histogram::draw() const
{
    const auto data = _get_data();
    size_t n_data   = data.size();
    float bar_width = 2.0f / static_cast<float>(n_data);
    float max_val   = static_cast<float>(*std::max_element(data.begin(), data.end()));

    glBegin(GL_QUADS);
    glColor4f(1.0f, 1.0f, 1.0f, 0.5f);
    float x_stt = -1.0f;
    for (auto val : data) {
        float x_stp = x_stt + bar_width;
        float y     = static_cast<float>(val) / max_val;

        glVertex2f(x_stt, 0.0f);
        glVertex2f(x_stp, 0.0f);
        glVertex2f(x_stp, y);
        glVertex2f(x_stt, y);

        x_stt = x_stp;
    }
    glEnd();

    return;
}