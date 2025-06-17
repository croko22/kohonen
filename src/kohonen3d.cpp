#include "kohonen3d.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cmath>

Kohonen3D::Kohonen3D(int x_dim, int y_dim, int z_dim, int input_size)
    : x_dim_(x_dim), y_dim_(y_dim), z_dim_(z_dim), input_size_(input_size),
      weight_dist_(0.0, 1.0)
{
    rng_.seed(std::random_device()());
    neurons_.resize(x_dim_,
                    std::vector<std::vector<SOMNeuron3D>>(y_dim_,
                                                          std::vector<SOMNeuron3D>(z_dim_)));
    initializeWeights();
}

std::vector<double> Kohonen3D::getWeights(int x, int y, int z) const
{
    if (x < 0 || x >= x_dim_ || y < 0 || y >= y_dim_ || z < 0 || z >= z_dim_)
    {
        throw std::out_of_range("Coordinates out of bounds");
    }
    return neurons_[x][y][z].weights;
}

void Kohonen3D::initializeWeights()
{
    for (int x = 0; x < x_dim_; ++x)
    {
        for (int y = 0; y < y_dim_; ++y)
        {
            for (int z = 0; z < z_dim_; ++z)
            {
                neurons_[x][y][z].weights.resize(input_size_);
                for (int i = 0; i < input_size_; ++i)
                {
                    neurons_[x][y][z].weights[i] = weight_dist_(rng_);
                }
            }
        }
    }
}

std::tuple<int, int, int> Kohonen3D::findBMU(const std::vector<double> &pattern) const
{
    double min_dist = std::numeric_limits<double>::max();
    std::tuple<int, int, int> bmu = {0, 0, 0};

    for (int x = 0; x < x_dim_; ++x)
    {
        for (int y = 0; y < y_dim_; ++y)
        {
            for (int z = 0; z < z_dim_; ++z)
            {
                double dist = 0.0;
                for (int i = 0; i < input_size_; ++i)
                {
                    double diff = pattern[i] - neurons_[x][y][z].weights[i];
                    dist += diff * diff;
                }
                if (dist < min_dist)
                {
                    min_dist = dist;
                    bmu = {x, y, z};
                }
            }
        }
    }

    return bmu;
}

double Kohonen3D::distance3D(int x1, int y1, int z1, int x2, int y2, int z2) const
{
    return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2) + std::pow(z1 - z2, 2));
}

void Kohonen3D::updateWeights(const std::vector<double> &pattern,
                              const std::tuple<int, int, int> &bmu,
                              double learning_rate, double radius)
{
    auto [bmu_x, bmu_y, bmu_z] = bmu;

    for (int x = 0; x < x_dim_; ++x)
    {
        for (int y = 0; y < y_dim_; ++y)
        {
            for (int z = 0; z < z_dim_; ++z)
            {
                double dist_to_bmu = distance3D(x, y, z, bmu_x, bmu_y, bmu_z);

                if (dist_to_bmu <= radius)
                {

                    double influence = std::exp(-(dist_to_bmu * dist_to_bmu) / (2 * radius * radius));

                    for (int i = 0; i < input_size_; ++i)
                    {
                        double delta = learning_rate * influence *
                                       (pattern[i] - neurons_[x][y][z].weights[i]);
                        neurons_[x][y][z].weights[i] += delta;
                    }
                }
            }
        }
    }
}

void Kohonen3D::train(const std::vector<std::vector<double>> &patterns,
                      int epochs, double initial_learning_rate)
{
    trained_patterns_ = patterns;
    double initial_radius = std::max({x_dim_, y_dim_, z_dim_}) / 2.0;
    double time_constant = epochs / log(initial_radius);

    for (int epoch = 0; epoch < epochs; ++epoch)
    {
        double learning_rate = initial_learning_rate * exp(-epoch / time_constant);
        double radius = initial_radius * exp(-epoch / time_constant);
        radius = std::max(1.0, radius);

        std::vector<std::vector<double>> shuffled_patterns = patterns;
        std::shuffle(shuffled_patterns.begin(), shuffled_patterns.end(), rng_);

        for (const auto &pattern : shuffled_patterns)
        {
            auto bmu = findBMU(pattern);
            updateWeights(pattern, bmu, learning_rate, radius);
        }
    }
}

std::tuple<int, int, int> Kohonen3D::predict(const std::vector<double> &pattern) const
{
    return findBMU(pattern);
}

void Kohonen3D::save(const std::string &filename) const
{
    std::ofstream out(filename);
    if (!out)
    {
        throw std::runtime_error("Cannot open file for writing: " + filename);
    }

    out << x_dim_ << "," << y_dim_ << "," << z_dim_ << "," << input_size_ << "\n";

    for (int x = 0; x < x_dim_; ++x)
    {
        for (int y = 0; y < y_dim_; ++y)
        {
            for (int z = 0; z < z_dim_; ++z)
            {
                const auto &weights = neurons_[x][y][z].weights;
                for (size_t i = 0; i < weights.size(); ++i)
                {
                    out << weights[i];
                    if (i != weights.size() - 1)
                        out << ",";
                }
                out << "\n";
            }
        }
    }
}

void Kohonen3D::load(const std::string &filename)
{
    std::ifstream in(filename);
    if (!in)
    {
        throw std::runtime_error("Cannot open file for reading: " + filename);
    }

    // Leer encabezado
    std::string header;
    std::getline(in, header);
    int x_dim = 0, y_dim = 0, z_dim = 0, input_size = 0;

    int result = sscanf(header.c_str(), "%d,%d,%d,%d", &x_dim, &y_dim, &z_dim, &input_size);
    if (result != 4)
    {
        throw std::runtime_error("Invalid header format. Expected 'x,y,z,input_size'");
    }

    x_dim_ = x_dim;
    y_dim_ = y_dim;
    z_dim_ = z_dim;
    input_size_ = input_size;

    neurons_.resize(x_dim_,
                    std::vector<std::vector<SOMNeuron3D>>(y_dim_,
                                                          std::vector<SOMNeuron3D>(z_dim_)));

    std::string line;
    int neuron_index = 0;

    while (std::getline(in, line) && neuron_index < x_dim_ * y_dim_ * z_dim_)
    {
        std::istringstream ss(line);
        std::string token;
        std::vector<double> weights;

        while (std::getline(ss, token, ','))
        {
            try
            {
                weights.push_back(std::stod(token));
            }
            catch (const std::invalid_argument &)
            {
                throw std::runtime_error("Error parsing weight at line " + std::to_string(neuron_index + 1));
            }
        }

        if (weights.size() != input_size_)
        {
            throw std::runtime_error("Line " + std::to_string(neuron_index + 1) +
                                     " has invalid number of weights (" +
                                     std::to_string(weights.size()) +
                                     "), expected " + std::to_string(input_size_));
        }

        int x = neuron_index / (y_dim_ * z_dim_);
        int y = (neuron_index / z_dim_) % y_dim_;
        int z = neuron_index % z_dim_;

        neurons_[x][y][z].weights = std::move(weights);
        ++neuron_index;
    }

    if (neuron_index < x_dim_ * y_dim_ * z_dim_)
    {
        throw std::runtime_error("File ended prematurely. Expected " +
                                 std::to_string(x_dim_ * y_dim_ * z_dim_) +
                                 " neurons, only read " +
                                 std::to_string(neuron_index));
    }

    std::cout << "Loaded " << neuron_index << " neurons successfully." << std::endl;
}