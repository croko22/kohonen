#include "visualization.h"
#include <GL/glut.h>
#include <iostream>
#include <algorithm>

KohonenVisualizer *KohonenVisualizer::s_instance = nullptr;

KohonenVisualizer::KohonenVisualizer(Kohonen3D *som)
    : som(som), zoom(10.0f), angleX(-20.0f), angleY(30.0f),
      mouseLeftDown(false), mouseX(0), mouseY(0),
      current_step(0), total_steps(1000)
{
    s_instance = this;
}

void KohonenVisualizer::displayCallback() { s_instance->display(); }
void KohonenVisualizer::reshapeCallback(int w, int h) { s_instance->reshape(w, h); }
void KohonenVisualizer::mouseCallback(int b, int s, int x, int y) { s_instance->mouse(b, s, x, y); }
void KohonenVisualizer::motionCallback(int x, int y) { s_instance->motion(x, y); }
void KohonenVisualizer::idleCallback() { s_instance->idle(); }

void KohonenVisualizer::initGL(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Kohonen Visualizer");

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    GLfloat lightPos[] = {1.0f, 1.0f, 1.0f, 0.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glutDisplayFunc(KohonenVisualizer::displayCallback);
    glutReshapeFunc(KohonenVisualizer::reshapeCallback);
    glutMouseFunc(KohonenVisualizer::mouseCallback);
    glutMotionFunc(KohonenVisualizer::motionCallback);
    glutIdleFunc(KohonenVisualizer::idleCallback);
}

void KohonenVisualizer::run()
{
    glutMainLoop();
}

void KohonenVisualizer::display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -zoom);
    glRotatef(angleX, 1.0f, 0.0f, 0.0f);
    glRotatef(angleY, 0.0f, 1.0f, 0.0f);

    float spacing = 0.5f;
    float sphereSize = 0.2f;

    for (int x = 0; x < som->x_dim_; ++x)
        for (int y = 0; y < som->y_dim_; ++y)
            for (int z = 0; z < som->z_dim_; ++z)
            {
                const auto &w = som->neurons_[x][y][z].weights;
                glColor3d(std::clamp(w[0], 0.0, 1.0),
                          std::clamp(w[1], 0.0, 1.0),
                          std::clamp(w[2], 0.0, 1.0));
                glPushMatrix();
                glTranslatef((x - 1) * spacing, (y - 1) * spacing, (z - 1) * spacing);
                glutSolidSphere(sphereSize, 20, 20);
                glPopMatrix();
            }

    glutSwapBuffers();
}

void KohonenVisualizer::reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / h, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void KohonenVisualizer::mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        mouseLeftDown = (state == GLUT_DOWN);
        mouseX = x;
        mouseY = y;
    }
    else if (button == 3 && state == GLUT_DOWN)
        zoom -= 0.5f;
    else if (button == 4 && state == GLUT_DOWN)
        zoom += 0.5f;

    glutPostRedisplay();
}

void KohonenVisualizer::motion(int x, int y)
{
    if (mouseLeftDown)
    {
        angleY += (x - mouseX);
        angleX += (y - mouseY);
        mouseX = x;
        mouseY = y;
        glutPostRedisplay();
    }
}

void KohonenVisualizer::idle()
{
    if (current_step < total_steps)
    {
        int idx = std::uniform_int_distribution<>(0, training_patterns.size() - 1)(som->rng_);
        som->updateWeights(training_patterns[idx], som->findBMU(training_patterns[idx]), 0.1, 1.0);
        current_step++;
        std::cout << "Step: " << current_step << " / " << total_steps << "\r" << std::flush;
        glutPostRedisplay();
    }
}
