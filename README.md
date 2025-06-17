# Kohonen 3D en C++ con OpenGL

## Descripcion

Red neuronal Kohonen en 3D con visualizacion OpenGL. Mapea datos de alta dimension a un espacio 3D conservando sus relaciones.

## Compilacion y uso

```bash
# Descargar y preparar la data
./scripts/start.sh

# Entrenar y mostrar la red
./main.sh

# Entrenar la red
make run

# Visualizar resultados
make run-visualize
```

## Controles

- Mouse: Rotar vista
- Scroll: Zoom
- WASD: Mover camara
- Espacio: Cambiar modo visual
- R: Resetear vista

## Configuracion

Modificar config.h para ajustar:

- Dimensiones de la red (X_DIM, Y_DIM, Z_DIM)
- Numero de epocas (EPOCHS)
- Tasa de aprendizaje (LEARNING_RATE)

## Estructura principal

- kohonen3d.h: Implementacion del algoritmo
- visualizer.h: Renderizado OpenGL
- main.cpp: Ejecucion principal

## Requerimientos

- Compilador C++17
- Bibliotecas OpenGL
- GLFW y GLAD instalados
