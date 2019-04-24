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
    float bar_width = 1.0f / static_cast<float>(n_data);
    float max_val   = static_cast<float>(*std::max_element(data.begin(), data.end()));

    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(-0.01f, 1.01f, -0.01f, 1.01f);

    for (auto render : {std::make_pair(GL_QUADS, Color{1.0f, 1.0f, 1.0f, 0.5f}),
                        std::make_pair(GL_LINE_LOOP, Color{1.0f, 1.0f, 1.0f, 1.0f})}) {
        
        float x_stt = 0.0f;
        for (auto val : data) {
            glBegin(render.first);
            glColor4f(render.second.r, render.second.g, render.second.b,
                      render.second.a);
            float x_stp = x_stt + bar_width;
            float y     = static_cast<float>(val) / max_val;

            glVertex2f(x_stt, 0.0f);
            glVertex2f(x_stp, 0.0f);
            glVertex2f(x_stp, y);
            glVertex2f(x_stt, y);

            glEnd();

			x_stt = x_stp;
        }
    }

    glPopMatrix();

    return;
}