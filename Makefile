CXX = g++
NVCC = nvcc
CXXFLAGS = -std=c++17 -O3
NVCCFLAGS = -std=c++17 -O3 -DUSE_CUDA
SRC = examples/train/main.cpp src/kohonen.cpp
CUDA_SRC = src/mlp_cuda.cu
TARGET = build/main

all: build $(TARGET)

build:
	mkdir -p build

$(TARGET): $(SRC) | build
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

cuda: $(SRC) $(CUDA_SRC) | build
	$(NVCC) $(NVCCFLAGS) -o $(TARGET) $(SRC) $(CUDA_SRC)

VIS_SRC = examples/visualize/main.cpp examples/visualize/visualization.cpp examples/visualize/training.cpp src/kohonen3d.cpp
VIS_TARGET = build/solids
VIS_LIBS = -lGL -lGLU -lglut

visualize: $(VIS_SRC) | build
	$(CXX) $(CXXFLAGS) -o $(VIS_TARGET) $(VIS_SRC) $(VIS_LIBS)

run-visualize: visualize
	./$(VIS_TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build