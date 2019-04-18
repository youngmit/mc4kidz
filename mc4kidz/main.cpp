#include <iostream>
#include <memory>

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include "GL/gl.h"
#include "GL/glut.h"

#include "simple_structs.h"
#include "state.h"

std::unique_ptr<State> state;
static const float WORLD_WIDTH  = 10.0f;
static const float WORLD_HEIGHT = 10.0f;
static const float WORLD_MARGIN = 0.1f;

void display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    state->draw();
    glFlush();
}

void timer(int paused)
{
    state->tic();
    glutPostRedisplay();
    glutTimerFunc(20, timer, paused);
}

void reshape(int width, int height)
{
    float aspect = (float)width / (float)height;

    glViewport(0, 0, width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Make sure that the smallest dimension shows the full domain in that direction
    if (width >= height) {
        gluOrtho2D(-WORLD_MARGIN, (WORLD_HEIGHT + WORLD_MARGIN) * aspect, -WORLD_MARGIN,
                   WORLD_WIDTH + WORLD_MARGIN);
    } else {
        gluOrtho2D(-WORLD_MARGIN, (WORLD_HEIGHT + WORLD_MARGIN), -WORLD_MARGIN,
                   (WORLD_WIDTH + WORLD_MARGIN) / aspect);
    }
    return;
}

void key(unsigned char key, int x, int y)
{
    switch (key) {
    case 'r':
        state->reset();
        break;
    case 'b':
        state->toggle_boundary_condition();
        break;
    case 'w':
        state->toggle_waypoints();
        break;
    case 'p':
        state->toggle_pause();
        break;
    case 'l':
        state->toggle_labels();
        glutPostRedisplay();
        break;
    case ' ':
        state->tic(true);
        glutPostRedisplay();
        break;
    }
    return;
}

void mouse(int button, int button_state, int x, int y)
{
    int window_width  = glutGet(GLUT_WINDOW_WIDTH);
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);

    // This is some nasty jazz. Should probably make a class to manage the
    // display...
    float scale = 0.0;
    if (window_width <= window_height) {
        float world_size = WORLD_WIDTH + 2.0f * WORLD_MARGIN;
        scale            = (float)window_width / world_size;
    } else {
        float world_size = WORLD_HEIGHT + 2.0f * WORLD_MARGIN;
        scale            = (float)window_height / world_size;
    }

    float world_x = (float)x / scale - WORLD_MARGIN;
    float world_y = (float)(window_height - y) / scale - WORLD_MARGIN;
    std::cout << button_state << " " << world_x << ", " << world_y << "\n";

    if (button == GLUT_RIGHT_BUTTON && button_state == GLUT_DOWN) {
        state->cycle_shape(world_x, world_y);
    }
    return;
}

int main(int argc, char *argv[])
{
    std::cout << "Here we go!\n";

    state = std::make_unique<State>();

    glutInit(&argc, argv);
    glutInitWindowSize(1024, 1024);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("MC 4 Kidz!");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key);
    glutMouseFunc(mouse);
    glutTimerFunc(20, timer, 0);
    glutMainLoop();
    return 0;
}
