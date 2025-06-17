#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cmath>
#include "../../src/kohonen3d.h"

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

void drawCube(GLuint tex, bool isWinner = false, float scaleFactor = 1.0f, float opacity = 1.0f)
{
    // Set opacity
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(1.0f, 1.0f, 1.0f, opacity);

    // Cambiar color si es el patrón ganador
    if (isWinner)
    {
        glColor4f(1.0f, 0.0f, 0.0f, 1.0f); // Rojo para el ganador (full opacity)
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    float size = isWinner ? 0.7f * scaleFactor : 0.5f * scaleFactor;

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

    // Back face
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

    // Right face
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

    // Left face
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
    glDisable(GL_BLEND);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f); // Resetear color
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
    gluPerspective(fov, 800.0 / 600.0, 0.1, 100.0);
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
    GLFWwindow *win = glfwCreateWindow(800, 600, "Fashion-MNIST 3D SOM - Test Image with BMU Connection", NULL, NULL);
    if (!win)
    {
        std::cerr << "Error: No se pudo crear la ventana GLFW" << std::endl;
        return -1;
    }
    glfwMakeContextCurrent(win);
    glewInit();

    glfwSetCursorPosCallback(win, mouseCallback);
    glfwSetScrollCallback(win, scrollCallback);
    glfwSetInputMode(win, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);

    // Configurar fondo negro para mejor contraste
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    // Configurar iluminación
    GLfloat lightColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat lightPos[] = {10.0f, 10.0f, 10.0f, 1.0f};
    GLfloat ambientLight[] = {0.4f, 0.4f, 0.4f, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

    // Cargar modelo SOM entrenado
    std::cout << "Cargando modelo SOM 3D..." << std::endl;
    Kohonen3D som(10, 10, 10, 784);
    som.load("models/afromnist/vai_kohonen_model_3D.bin");
    std::cout << "Modelo cargado exitosamente!" << std::endl;

    // Cargar datos de test (solo 1 imagen)
    std::cout << "Cargando datos de test..." << std::endl;
    auto test_data = loadImages("data/Vai/mnist_test.csv", 1);
    if (test_data.empty())
    {
        std::cerr << "No se pudo cargar la imagen de test." << std::endl;
        return 1;
    }
    std::cout << "Cargada 1 muestra de test" << std::endl;

    // Crear textura para la imagen de test
    GLuint test_texture = createTexture(test_data[0]);

    // Crear texturas para toda la grilla 10x10x10 usando los pesos del SOM
    std::vector<std::vector<std::vector<GLuint>>> grid_textures(10,
                                                                std::vector<std::vector<GLuint>>(10, std::vector<GLuint>(10)));

    // Generar texturas desde los pesos de cada neurona
    for (int x = 0; x < 10; ++x)
    {
        for (int y = 0; y < 10; ++y)
        {
            for (int z = 0; z < 10; ++z)
            {
                // Obtener pesos de la neurona en posición (x,y,z)
                auto weights = som.getWeights(x, y, z);

                // Convertir pesos a imagen 28x28
                std::vector<unsigned char> neuron_image(784);
                for (int i = 0; i < 784; ++i)
                {
                    // Normalizar peso a rango 0-255
                    double normalized = std::max(0.0, std::min(255.0, weights[i] * 255.0));
                    neuron_image[i] = static_cast<unsigned char>(normalized);
                }

                grid_textures[x][y][z] = createTexture(neuron_image);
            }
        }
    }

    // Obtener BMU para la imagen de test
    std::vector<double> doublePattern(test_data[0].begin(), test_data[0].end());
    auto bmu_pos = som.predict(doublePattern);
    int bmu_x = std::get<0>(bmu_pos);
    int bmu_y = std::get<1>(bmu_pos);
    int bmu_z = std::get<2>(bmu_pos);

    std::cout << "BMU encontrado en posición: (" << bmu_x << ", " << bmu_y << ", " << bmu_z << ")" << std::endl;

    // Posición para mostrar la imagen de test (a la izquierda de la grilla)
    float test_img_x = -12.0f;
    float test_img_y = 0.0f;
    float test_img_z = 0.0f;

    std::cout << "Iniciando visualización..." << std::endl;
    std::cout << "Controles:" << std::endl;
    std::cout << "- Mouse: Rotar cámara" << std::endl;
    std::cout << "- Scroll: Zoom in/out" << std::endl;
    std::cout << "- ESC: Salir" << std::endl;

    while (!glfwWindowShouldClose(win) && !glfwGetKey(win, GLFW_KEY_ESCAPE))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        setupCamera();

        // Dibujar la imagen de test (más grande)
        glPushMatrix();
        glTranslatef(test_img_x, test_img_y, test_img_z);
        drawCube(test_texture, false, 1.5f); // Escala más grande para la imagen de test
        glPopMatrix();

        // DIBUJAR TODA LA GRILLA 10x10x10 CON LOS PESOS DEL SOM
        float grid_scale = 1.2f;
        for (int x = 0; x < 10; ++x)
        {
            for (int y = 0; y < 10; ++y)
            {
                for (int z = 0; z < 10; ++z)
                {
                    bool isBMU = (x == bmu_x && y == bmu_y && z == bmu_z);
                    float opacity = isBMU ? 1.0f : 0.3f; // Full opacity for BMU, 30% for others

                    glPushMatrix();
                    glTranslatef((x - 4.5f) * grid_scale, (y - 4.5f) * grid_scale, (z - 4.5f) * grid_scale);
                    drawCube(grid_textures[x][y][z], isBMU, 1.0f, opacity);
                    glPopMatrix();
                }
            }
        }

        // Dibujar línea conectando la imagen de test con su BMU
        glDisable(GL_LIGHTING);
        glLineWidth(3.0f);
        glColor3f(0.0f, 1.0f, 0.0f); // Verde brillante

        glBegin(GL_LINES);
        // Punto en la imagen de test
        glVertex3f(test_img_x, test_img_y, test_img_z);
        // Punto en el BMU
        glVertex3f((bmu_x - 4.5f) * grid_scale, (bmu_y - 4.5f) * grid_scale, (bmu_z - 4.5f) * grid_scale);
        glEnd();

        // Dibujar punto en el BMU
        glPointSize(10.0f);
        glColor3f(1.0f, 1.0f, 0.0f); // Amarillo brillante
        glBegin(GL_POINTS);
        glVertex3f((bmu_x - 4.5f) * grid_scale, (bmu_y - 4.5f) * grid_scale, (bmu_z - 4.5f) * grid_scale);
        glEnd();

        glEnable(GL_LIGHTING);

        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    // Limpiar texturas
    glDeleteTextures(1, &test_texture);
    for (int x = 0; x < 10; ++x)
    {
        for (int y = 0; y < 10; ++y)
        {
            for (int z = 0; z < 10; ++z)
            {
                glDeleteTextures(1, &grid_textures[x][y][z]);
            }
        }
    }

    glfwDestroyWindow(win);
    glfwTerminate();

    std::cout << "Visualización finalizada." << std::endl;
    return 0;
}