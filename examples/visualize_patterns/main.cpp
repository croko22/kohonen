#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>

float yaw = 0.0f, pitch = 0.0f, lastX = 320, lastY = 240, fov = 45.0f;
bool firstMouse = true;
float cameraDistance = 30.0f;

std::vector<std::vector<unsigned char>> loadImages(const std::string &filename, int count)
{
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line);
    std::vector<std::vector<unsigned char>> all;

    while (std::getline(file, line) && all.size() < count)
    {
        std::vector<unsigned char> pixels(28 * 28);
        std::stringstream ss(line);
        std::string item;
        std::getline(ss, item, ','); // label
        for (int i = 0; i < 784; ++i)
        {
            std::getline(ss, item, ',');
            pixels[i] = static_cast<unsigned char>(std::stoi(item));
        }
        all.push_back(pixels);
    }
    return all;
}

GLuint createTexture(const std::vector<unsigned char> &pixels)
{
    std::vector<unsigned char> rgb(28 * 28 * 3);
    for (int i = 0; i < 784; ++i)
    {
        unsigned char v = pixels[i];
        rgb[i * 3 + 0] = v;
        rgb[i * 3 + 1] = v;
        rgb[i * 3 + 2] = v;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 28, 28, 0, GL_RGB, GL_UNSIGNED_BYTE, rgb.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    return tex;
}

void drawCube(GLuint tex)
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    float size = 0.5f; // Smaller cubes for better visibility

    // Front face
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1);
    glVertex3f(-size, -size, size);
    glTexCoord2f(0, 1);
    glVertex3f(size, -size, size);
    glTexCoord2f(0, 0);
    glVertex3f(size, size, size);
    glTexCoord2f(1, 0);
    glVertex3f(-size, size, size);
    glEnd();

    // Back face (rotated texture 180 degrees)
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1);
    glVertex3f(size, -size, -size);
    glTexCoord2f(0, 1);
    glVertex3f(-size, -size, -size);
    glTexCoord2f(0, 0);
    glVertex3f(-size, size, -size);
    glTexCoord2f(1, 0);
    glVertex3f(size, size, -size);
    glEnd();

    // Top face
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-size, size, size);
    glTexCoord2f(1, 0);
    glVertex3f(size, size, size);
    glTexCoord2f(1, 1);
    glVertex3f(size, size, -size);
    glTexCoord2f(0, 1);
    glVertex3f(-size, size, -size);
    glEnd();

    // Bottom face
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex3f(-size, -size, -size);
    glTexCoord2f(1, 0);
    glVertex3f(size, -size, -size);
    glTexCoord2f(1, 1);
    glVertex3f(size, -size, size);
    glTexCoord2f(0, 1);
    glVertex3f(-size, -size, size);
    glEnd();

    // Right face (rotated texture 180 degrees)
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1);
    glVertex3f(size, -size, size);
    glTexCoord2f(0, 1);
    glVertex3f(size, -size, -size);
    glTexCoord2f(0, 0);
    glVertex3f(size, size, -size);
    glTexCoord2f(1, 0);
    glVertex3f(size, size, size);
    glEnd();

    // Left face (rotated texture 180 degrees)
    glBegin(GL_QUADS);
    glTexCoord2f(1, 1);
    glVertex3f(-size, -size, -size);
    glTexCoord2f(0, 1);
    glVertex3f(-size, -size, size);
    glTexCoord2f(0, 0);
    glVertex3f(-size, size, size);
    glTexCoord2f(1, 0);
    glVertex3f(-size, size, -size);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void mouseCallback(GLFWwindow *, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    yaw += xoffset * 0.2f;
    pitch += yoffset * 0.2f;
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}

void scrollCallback(GLFWwindow *, double, double yoffset)
{
    cameraDistance -= yoffset;
    if (cameraDistance < 5.0f)
        cameraDistance = 5.0f;
    if (cameraDistance > 100.0f)
        cameraDistance = 100.0f;
}

void setupCamera()
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, 640.0 / 480.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    float radYaw = yaw * M_PI / 180.0;
    float radPitch = pitch * M_PI / 180.0;

    float cx = cameraDistance * cos(radPitch) * sin(radYaw);
    float cy = cameraDistance * sin(radPitch);
    float cz = cameraDistance * cos(radPitch) * cos(radYaw);

    gluLookAt(cx, cy, cz, 0, 0, 0, 0, 1, 0);
}

int main()
{
    if (!glfwInit())
        return -1;
    GLFWwindow *win = glfwCreateWindow(640, 480, "MNIST Cube", NULL, NULL);
    if (!win)
        return -1;
    glfwMakeContextCurrent(win);
    glewInit();

    glfwSetCursorPosCallback(win, mouseCallback);
    glfwSetScrollCallback(win, scrollCallback);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // Set white background
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Configure lighting for better contrast
    GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightPos[] = {10.0f, 10.0f, 10.0f, 1.0f};
    GLfloat ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

    auto imgs = loadImages("models/kohonen_model_3D.bin", 1000);
    // auto imgs = loadImages("data/fashion-mnist_test.csv", 1000);
    std::vector<GLuint> textures;
    for (auto &img : imgs)
        textures.push_back(createTexture(img));

    while (!glfwWindowShouldClose(win))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        setupCamera();

        // Draw 10x10x10 grid of cubes without rotation
        for (int x = 0; x < 10; ++x)
            for (int y = 0; y < 10; ++y)
                for (int z = 0; z < 10; ++z)
                {
                    int idx = x * 100 + y * 10 + z;
                    if (idx >= textures.size())
                        break;
                    glPushMatrix();
                    glTranslatef((x - 4.5f) * 1.2f, (y - 4.5f) * 1.2f, (z - 4.5f) * 1.2f);
                    drawCube(textures[idx]);
                    glPopMatrix();
                }

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    // Clean up textures
    for (GLuint tex : textures)
        glDeleteTextures(1, &tex);

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}