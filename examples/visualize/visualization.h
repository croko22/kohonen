#pragma once

#include <GL/glut.h>
#include <vector>
#include "../../src/kohonen3d.h"

class KohonenVisualizer
{
public:
    KohonenVisualizer(Kohonen3D *som);
    static KohonenVisualizer *s_instance;
    void initGL(int argc, char **argv);
    void display();
    void reshape(int w, int h);
    void mouse(int button, int state, int x, int y);
    void motion(int x, int y);
    void idle();
    void run();

    static void displayCallback();
    static void reshapeCallback(int w, int h);
    static void mouseCallback(int b, int s, int x, int y);
    static void motionCallback(int x, int y);
    static void idleCallback();

    std::vector<std::vector<double>> training_patterns;

private:
    Kohonen3D *som;

    float zoom;
    float angleX, angleY;
    bool mouseLeftDown;
    int mouseX, mouseY;
    int current_step;
    int total_steps;
    int initial_radius;
    double initial_learning_rate;
};
