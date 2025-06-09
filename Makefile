CXX = g++
NVCC = nvcc
CXXFLAGS = -std=c++17 -O3
NVCCFLAGS = -std=c++17 -O3 -DUSE_CUDA
SRC = src/main.cpp src/kohonen.cpp
CUDA_SRC = src/mlp_cuda.cu
TARGET = build/main

all: build $(TARGET)

build:
	mkdir -p build

$(TARGET): $(SRC) | build
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

cuda: $(SRC) $(CUDA_SRC) | build
	$(NVCC) $(NVCCFLAGS) -o $(TARGET) $(SRC) $(CUDA_SRC)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build