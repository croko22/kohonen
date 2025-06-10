// #include <GL/glut.h>
// #include <iostream>
// #include <vector>
// #include <chrono>
// #include <thread>
// #include <random>
// #include <algorithm>
// #include <cmath>
// #include "../../src/kohonen3d.h"

// static float s_zoom = 10.0f;
// static float s_angleX = -20.0f;
// static float s_angleY = 30.0f;
// static int s_mouseX = 0;
// static int s_mouseY = 0;
// static bool s_mouseLeftDown = false;

// const int EPOCHS = 50;
// const int TOTAL_STEPS = 1000;
// int current_step = 0;

// std::vector<std::vector<double>> training_patterns;
// Kohonen3D *som = nullptr;

// void generate_simple_patterns()
// {

//     training_patterns.push_back({1.0, 0.0, 0.0});
//     training_patterns.push_back({0.0, 1.0, 0.0});
//     training_patterns.push_back({0.0, 0.0, 1.0});
//     training_patterns.push_back({1.0, 1.0, 0.0});
//     training_patterns.push_back({0.5, 0.5, 0.5});
// }

// void display()
// {
//     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//     glMatrixMode(GL_MODELVIEW);
//     glLoadIdentity();

//     glTranslatef(0.0f, 0.0f, -s_zoom);
//     glRotatef(s_angleX, 1.0f, 0.0f, 0.0f);
//     glRotatef(s_angleY, 0.0f, 1.0f, 0.0f);

//     float spacing = 0.5f;
//     float sphereSize = 0.2f;

//     for (int x = 0; x < som->x_dim_; ++x)
//     {
//         for (int y = 0; y < som->y_dim_; ++y)
//         {
//             for (int z = 0; z < som->z_dim_; ++z)
//             {
//                 const auto &weights = som->neurons_[x][y][z].weights;

//                 double r = std::clamp(weights[0], 0.0, 1.0);
//                 double g = std::clamp(weights[1], 0.0, 1.0);
//                 double b = std::clamp(weights[2], 0.0, 1.0);

//                 glPushMatrix();
//                 glTranslatef((x - 1) * spacing, (y - 1) * spacing, (z - 1) * spacing);
//                 glColor3d(r, g, b);
//                 glutSolidSphere(sphereSize, 20, 20);
//                 glPopMatrix();
//             }
//         }
//     }

//     glutSwapBuffers();
// }

// void reshape(GLint w, GLint h)
// {
//     glViewport(0, 0, w, h);
//     glMatrixMode(GL_PROJECTION);
//     glLoadIdentity();
//     gluPerspective(45.0, (float)w / (float)h, 0.1f, 100.0f);
//     glMatrixMode(GL_MODELVIEW);
// }

// void mouse(int button, int state, int x, int y)
// {
//     if (button == GLUT_LEFT_BUTTON)
//     {
//         s_mouseLeftDown = (state == GLUT_DOWN);
//         s_mouseX = x;
//         s_mouseY = y;
//     }
//     else if (button == 3 && state == GLUT_DOWN)
//     {
//         s_zoom -= 0.5f;
//         glutPostRedisplay();
//     }
//     else if (button == 4 && state == GLUT_DOWN)
//     {
//         s_zoom += 0.5f;
//         glutPostRedisplay();
//     }
// }

// void motion(int x, int y)
// {
//     if (s_mouseLeftDown)
//     {
//         s_angleY += (x - s_mouseX);
//         s_angleX += (y - s_mouseY);
//         s_mouseX = x;
//         s_mouseY = y;
//         glutPostRedisplay();
//     }
// }

// void train_step()
// {
//     static std::uniform_int_distribution<> distrib(0, training_patterns.size() - 1);
//     int idx = distrib(som->rng_);
//     const auto &pattern = training_patterns[idx];

//     som->updateWeights(pattern, som->findBMU(pattern), 0.1, 1.0);
// }

// void idle()
// {
//     static auto last_time = std::chrono::high_resolution_clock::now();
//     auto now = std::chrono::high_resolution_clock::now();
//     auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_time).count();

//     if (elapsed > 30)
//     {
//         if (current_step < TOTAL_STEPS)
//         {
//             train_step();
//             ++current_step;
//             std::cout << "Step: " << current_step << " / " << TOTAL_STEPS << "\r";
//             std::flush(std::cout);
//         }
//         last_time = now;
//         glutPostRedisplay();
//     }
// }

// void init()
// {
//     glEnable(GL_LIGHTING);
//     glEnable(GL_LIGHT0);
//     glEnable(GL_DEPTH_TEST);
//     glEnable(GL_COLOR_MATERIAL);
//     glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

//     GLfloat light_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
//     GLfloat light_diffuse[] = {0.8f, 0.8f, 0.8f, 1.0f};
//     GLfloat light_specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
//     GLfloat light_position[] = {1.0f, 1.0f, 1.0f, 0.0f};

//     glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
//     glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
//     glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
//     glLightfv(GL_LIGHT0, GL_POSITION, light_position);

//     glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
// }

// int main(int argc, char **argv)
// {
//     generate_simple_patterns();

//     som = new Kohonen3D(3, 3, 3, training_patterns[0].size());

//     som->train(training_patterns, 1, 0.1);

//     glutInit(&argc, argv);
//     glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
//     glutInitWindowSize(800, 600);
//     glutCreateWindow("Kohonen 3D Visualization");

//     glutDisplayFunc(display);
//     glutReshapeFunc(reshape);
//     glutMouseFunc(mouse);
//     glutMotionFunc(motion);
//     glutIdleFunc(idle);

//     init();
//     glutMainLoop();

//     delete som;
//     return 0;
// }

#include "visualization.h"
#include "training.h"

int main(int argc, char **argv)
{
    auto patterns = KohonenTraining::generateSimplePatterns();
    Kohonen3D *som = new Kohonen3D(3, 3, 3, patterns[0].size());
    KohonenVisualizer visualizer(som);
    visualizer.training_patterns = patterns;
    visualizer.initGL(argc, argv);
    visualizer.run();
    delete som;
    return 0;
}
