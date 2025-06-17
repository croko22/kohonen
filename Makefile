CXX = g++
NVCC = nvcc
CXXFLAGS = -std=c++17 -O3
NVCCFLAGS = -std=c++17 -O3 -DUSE_CUDA
SRC = examples/train/main.cpp src/kohonen.cpp
CUDA_SRC = src/mlp_cuda.cu
TARGET = build/main

FILE ?= input.txt

all: build $(TARGET)

build:
	mkdir -p build

$(TARGET): $(SRC) | build
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

cuda: $(SRC) $(CUDA_SRC) | build
	$(NVCC) $(NVCCFLAGS) -o $(TARGET) $(SRC) $(CUDA_SRC)

# TRAINING
TRAIN3D_SRC = examples/train/train3D.cpp src/kohonen3d.cpp
TRAIN3D_TARGET = build/train3d

TRAIN2D_SRC = examples/train/train2D.cpp src/kohonen.cpp
TRAIN2D_TARGET = build/train2d

train3d: $(TRAIN3D_SRC) | build
	$(CXX) $(CXXFLAGS) -o $(TRAIN3D_TARGET) $(TRAIN3D_SRC)

run-train3d: train3d
	./$(TRAIN3D_TARGET) $(FILE)

train2d: $(TRAIN2D_SRC) | build
	$(CXX) $(CXXFLAGS) -o $(TRAIN2D_TARGET) $(TRAIN2D_SRC)

run-train2d: train2d
	./$(TRAIN2D_TARGET)

# VISUALIZATION
SIMPLE_VIS_SRC = examples/visualize/simple_example.cpp examples/visualize/visualization.cpp examples/visualize/training.cpp src/kohonen3d.cpp
VIS_SRC = examples/visualize/main.cpp examples/visualize/visualization.cpp examples/visualize/training.cpp src/kohonen3d.cpp
VIS_TARGET = build/solids
VIS_LIBS = -lGL -lGLU -lglut

simple-visualize: $(SIMPLE_VIS_SRC) | build
	$(CXX) $(CXXFLAGS) -o $(VIS_TARGET) $(SIMPLE_VIS_SRC) $(VIS_LIBS)
 
visualize: $(VIS_SRC) | build
	$(CXX) $(CXXFLAGS) -o $(VIS_TARGET) $(VIS_SRC) $(VIS_LIBS)

run-simple-visualize: simple-visualize
	./$(VIS_TARGET)

run-visualize: visualize
	./$(VIS_TARGET)

run: $(TARGET)
	./$(TARGET)


VISUALIZE_PATTERN_SRC = examples/visualize_patterns/main.cpp
VISUALIZE_PATTERN_TARGET = build/viewer
VISUALIZE_PATTERN_LIBS = -lGL -lglfw -lGLEW -lGLU


viewer: $(VISUALIZE_PATTERN_SRC) | build
	$(CXX) $(CXXFLAGS) -o $(VISUALIZE_PATTERN_TARGET) $(VISUALIZE_PATTERN_SRC) $(VISUALIZE_PATTERN_LIBS)

run-train-visualizer: viewer
	./$(VISUALIZE_PATTERN_TARGET) $(FILE)