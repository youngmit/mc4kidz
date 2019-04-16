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
        gluOrtho2D(-0.1f, 10.1 * aspect, -0.1f, 10.1f);
    } else {
        gluOrtho2D(-0.1, 10.1, -0.1, 10.1 / aspect);
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
    case ' ':
        state->tic(true);
        glutPostRedisplay();
        break;
    }
    return;
}

int main(int argc, char *argv[])
{
    std::cout << "Here we go!\n";

    Vec2 p1{0.0, 0.0};
    Vec2 p2{1.0, 1.0};
    p1 = p1 + p2;

    state = std::make_unique<State>();

    glutInit(&argc, argv);
    glutInitWindowSize(1024, 1024);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("MC 4 Kidz!");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key);
    glutTimerFunc(20, timer, 0);
    glutMainLoop();
    return 0;
}
