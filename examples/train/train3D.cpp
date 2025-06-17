#include "../../src/kohonen3d.h"
#include "../../src/utils/utils.cpp"
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>

int main(int argc, char* argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Uso: " << argv[0] << " <ruta_base_csv>" << std::endl;
            std::cerr << "Ejemplo: " << argv[0] << " data/fashion-mnist" << std::endl;
            return 1;
        }

        std::string base_path = argv[1];
        std::string train_path = base_path + "_train.csv";
        std::string test_path = base_path + "_test.csv";

        std::cout << "Loading training data from: " << train_path << std::endl;
        auto train_data = load_mnist_csv(train_path);

        std::cout << "Loading test data from: " << test_path << std::endl;
        auto test_data = load_mnist_csv(test_path);

        std::cout << "Training data loaded: " << train_data.images.size() << " samples." << std::endl;
        std::cout << "Test data loaded: " << test_data.images.size() << " samples." << std::endl;

        auto train_patterns = convert_to_bipolar(train_data.images);
        auto test_patterns = convert_to_bipolar(test_data.images);

        int pattern_size = train_patterns[0].size();
        std::cout << "Pattern size: " << pattern_size << " (28x28 = 784 pixels)" << std::endl;

        std::vector<std::vector<double>> selected_patterns;
        for (int class_id = 0; class_id < 10; ++class_id)
        {
            int count = 0;
            for (size_t i = 0; i < train_data.labels.size() && count < 10; ++i)
            {
                if (train_data.labels[i] == class_id)
                {
                    selected_patterns.push_back(train_patterns[i]);
                    count++;
                }
            }
            std::cout << "Class " << class_id << ": " << count << " patterns selected" << std::endl;
        }

        Kohonen3D som(10, 10, 10, pattern_size);

        std::cout << "Training Kohonen network with " << selected_patterns.size()
                  << " patterns (10 per class)..." << std::endl;

        auto start = std::chrono::high_resolution_clock::now();
        som.train(selected_patterns, 50, 0.5);
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Training completed in " << elapsed.count() << " seconds" << std::endl;

        som.save("models/kohonen_model_3D.bin");
        std::cout << "Model saved to kohonen_model_3D.bin" << std::endl;

        std::cout << "\nTesting with evaluation data:" << std::endl;
        for (int class_id = 0; class_id < 10; ++class_id)
        {
            size_t idx = 0;
            while (idx < test_data.labels.size() && test_data.labels[idx] != class_id)
                ++idx;

            if (idx >= test_data.labels.size())
                continue;

            const auto &test_pattern = test_patterns[idx];
            auto winner = som.predict(test_pattern);

            std::cout << "Class " << class_id << " (" << get_class_name(class_id)
                      << "): Winning neuron (" << std::get<0>(winner) << ", "
                      << std::get<1>(winner) << ", "
                      << std::get<2>(winner) << ")" << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}