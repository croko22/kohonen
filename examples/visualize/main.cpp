#include <iostream>
#include <vector>
#include <chrono>

#include "visualization.h"
#include "training.h"
#include "../../src/utils/utils.cpp"

int main(int argc, char **argv)
{
    try
    {
        std::cout << "Loading Fashion MNIST data from CSV..." << std::endl;
        auto train_data = load_mnist_csv("data/fashion-mnist_train.csv");
        if (train_data.images.empty())
        {
            throw std::runtime_error("Could not load or find training data CSV.");
        }

        auto train_patterns = convert_to_bipolar(train_data.images);
        int pattern_size = train_patterns[0].size();

        std::cout << "Data loaded: " << train_patterns.size() << " training patterns." << std::endl;
        std::cout << "Pattern size: " << pattern_size << std::endl;

        Kohonen3D *som = new Kohonen3D(10, 10, 10, pattern_size);
        KohonenVisualizer visualizer(som);

        visualizer.training_patterns = train_patterns;
        visualizer.initGL(argc, argv);
        visualizer.run();

        delete som;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}