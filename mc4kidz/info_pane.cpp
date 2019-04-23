#include "info_pane.h"

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include "GL/gl.h"
#include "GL/glut.h"

void InfoPane::draw() const
{
    int window_width    = glutGet(GLUT_WINDOW_WIDTH);
    int window_height   = glutGet(GLUT_WINDOW_HEIGHT);
    int pixels_from_top = 0;
    for (const auto &info : _info) {
		// for now assume all square
        glViewport(window_width - _width, window_height - pixels_from_top - _width,
                   _width, _width);
        info->draw();
        pixels_from_top += _width;
    }
    return;
}