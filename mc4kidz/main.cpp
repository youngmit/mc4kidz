#include <iostream>
#include <memory>

#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include "GL/gl.h"
#include "GL/glut.h"

#include "histogram.h"
#include "info_pane.h"
#include "line_plot.h"
#include "pie_chart.h"
#include "playbook.h"
#include "shapes.h"
#include "simple_structs.h"
#include "state.h"
#include "view.h"

static const float WORLD_WIDTH   = 17.0f;
static const float WORLD_HEIGHT  = 17.0f;
static const float WORLD_MARGIN  = 0.1f;
static const int INFO_PANE_WIDTH = 150;

std::unique_ptr<State> state;
std::unique_ptr<InfoPane> info_pane;

void display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    int window_width  = glutGet(GLUT_WINDOW_WIDTH);
    int window_height = glutGet(GLUT_WINDOW_HEIGHT);

    glViewport(0, 0, window_width, window_height);

    state->draw();
    info_pane->draw();

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

    // Make sure that the smallest dimension shows the full domain in that direction
    if (width >= height) {
        Ortho2D view = {-WORLD_MARGIN, (WORLD_HEIGHT + WORLD_MARGIN) * aspect,
                        -WORLD_MARGIN, WORLD_WIDTH + WORLD_MARGIN};
        state->set_view(view);
    } else {
        state->set_view(Ortho2D{-WORLD_MARGIN, (WORLD_HEIGHT + WORLD_MARGIN),
                                -WORLD_MARGIN, (WORLD_WIDTH + WORLD_MARGIN) / aspect});
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
    case 'c':
        state->cycle_all();
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

    if (button == GLUT_RIGHT_BUTTON && button_state == GLUT_DOWN) {
        state->cycle_shape(world_x, world_y);
    }

    if (button == GLUT_LEFT_BUTTON && button_state == GLUT_DOWN) {
        state->set_source(world_x, world_y);
    }

    if (button == GLUT_LEFT_BUTTON && button_state == GLUT_UP) {
        state->unset_source();
    }
    return;
}

void mouse_drag(int x, int y)
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

    if (state->has_source()) {
        state->set_source(world_x, world_y);
    }
    return;
}

int main(int argc, char *argv[])
{
    std::cout << "Here we go!\n";


    state     = std::make_unique<State>();
    info_pane = std::make_unique<InfoPane>(200, 10);
    info_pane->add_info(std::make_unique<InteractionPieChart>(state.get()));
    info_pane->add_info(std::make_unique<SpectrumHistogram>(state.get()));
    info_pane->add_info(std::make_unique<PopulationLinePlot>(state.get()));

	if (argc > 1) {
        std::string playbook_file = argv[1];
        state->set_playbook(std::make_unique<Playbook>(playbook_file));
    }

    glutInit(&argc, argv);
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(50, 50);
    glutCreateWindow("MC 4 Kidz!");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key);
    glutMouseFunc(mouse);
    glutMotionFunc(mouse_drag);
    glutTimerFunc(20, timer, 0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glutMainLoop();
    return 0;
}
