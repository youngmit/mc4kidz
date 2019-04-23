#include "pie_chart.h"

#include <numeric>
#include <vector>

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include "GL/freeglut.h"
#include "GL/gl.h"

#include "simple_structs.h"

#ifndef M_PI
static const float M_PI = 3.14159265359f;
#endif

static const std::vector<Color> _colors = {
    Color{1.0f, 0.0f, 0.0f, 0.3f}, Color{0.0f, 1.0f, 0.0f, 0.3f},
    Color{0.0f, 0.0f, 1.0f, 0.3f}, Color{1.0f, 1.0f, 0.0f, 0.3f}};

template <typename T> void PieChart<T>::draw() const
{
    const auto data          = get_data();
    const int total_segments = 100;
    T total                  = std::accumulate(data.begin(), data.end(), 0);
    float stt_angle          = 0.0f;
    for (int i = 0; i < data.size(); ++i) {
        float fraction   = static_cast<float>(data[i]) / static_cast<float>(total);
        int segments     = std::max(1, static_cast<int>(fraction * total_segments));
        float angle_span = fraction * 2.0f * M_PI;
        float stp_angle  = stt_angle + angle_span;

        auto color = _colors[i % _colors.size()];
        glBegin(GL_TRIANGLE_FAN);
        glColor4f(color.r, color.g, color.b, color.a);
        glVertex2f(0.0f, 0.0f);
        for (int i_seg = 0; i_seg <= segments; ++i_seg) {
            float angle = stt_angle + (i_seg / (float)segments) * angle_span;
            glVertex2f(cos(angle), sin(angle));
        }

        glEnd();
        stt_angle = stp_angle;
    }
    return;
}

template class PieChart<unsigned int>;