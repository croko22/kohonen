#include "kohonen.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <stdexcept>

Kohonen::Kohonen(int rows, int cols, int input_size)
    : rows_(rows), cols_(cols), input_size_(input_size),
      neurons_(rows, std::vector<SOMNeuron>(cols)),
      rng_(std::random_device{}()),
      weight_dist_(-1.0, 1.0)
{
    initializeWeights();
}

void Kohonen::initializeWeights()
{
    for (auto &row : neurons_)
    {
        for (auto &neuron : row)
        {
            neuron.weights.resize(input_size_);
            for (auto &w : neuron.weights)
            {
                w = weight_dist_(rng_);
            }
        }
    }
}

void Kohonen::train(const std::vector<std::vector<double>> &patterns, int epochs, double initial_learning_rate)
{
    if (patterns.empty())
        throw std::invalid_argument("No training patterns provided.");

    trained_patterns_ = patterns;

    double time_constant = static_cast<double>(epochs) / std::log(static_cast<double>(std::max(rows_, cols_)));
    double initial_radius = std::max(rows_, cols_) / 2.0;

    for (int epoch = 0; epoch < epochs; ++epoch)
    {
        double t = static_cast<double>(epoch + 1);
        double learning_rate = initial_learning_rate * std::exp(-t / time_constant);
        double radius = initial_radius * std::exp(-t / time_constant);

        for (const auto &pattern : patterns)
        {
            auto bmu = findBMU(pattern);
            updateWeights(pattern, bmu, learning_rate, radius);
        }

        if ((epoch + 1) % 10 == 0 || epoch == 0)
        {
            std::cout << "Epoch " << (epoch + 1) << " completada." << std::endl;
        }
    }
}

std::pair<int, int> Kohonen::findBMU(const std::vector<double> &pattern) const
{
    double min_dist = std::numeric_limits<double>::max();
    std::pair<int, int> bmu(0, 0);

    for (int i = 0; i < rows_; ++i)
    {
        for (int j = 0; j < cols_; ++j)
        {
            double dist = 0.0;
            const auto &weights = neurons_[i][j].weights;
            for (size_t k = 0; k < weights.size(); ++k)
            {
                double diff = pattern[k] - weights[k];
                dist += diff * diff;
            }
            if (dist < min_dist)
            {
                min_dist = dist;
                bmu = {i, j};
            }
        }
    }
    return bmu;
}

void Kohonen::updateWeights(const std::vector<double> &pattern, const std::pair<int, int> &bmu,
                            double learning_rate, double radius)
{
    double radius_sq = radius * radius;
    for (int i = 0; i < rows_; ++i)
    {
        for (int j = 0; j < cols_; ++j)
        {
            double dx = i - bmu.first;
            double dy = j - bmu.second;
            double dist_sq = dx * dx + dy * dy;

            if (dist_sq <= radius_sq)
            {
                double influence = std::exp(-dist_sq / (2 * radius_sq));
                auto &weights = neurons_[i][j].weights;
                for (size_t k = 0; k < weights.size(); ++k)
                {
                    weights[k] += influence * learning_rate * (pattern[k] - weights[k]);
                }
            }
        }
    }
}

std::pair<int, int> Kohonen::predict(const std::vector<double> &pattern) const
{
    return findBMU(pattern);
}

void Kohonen::save(const std::string &filename) const
{
    std::ofstream file(filename);
    if (!file)
    {
        throw std::runtime_error("Could not open the file to save the model: " + filename);
    }

    file << rows_ << "," << cols_ << "," << input_size_ << "\n";

    for (const auto &row_vec : neurons_)
    {
        for (const auto &neuron : row_vec)
        {
            for (size_t i = 0; i < neuron.weights.size(); ++i)
            {
                file << neuron.weights[i] << (i == neuron.weights.size() - 1 ? "" : ",");
            }
            file << "\n";
        }
    }
}

void Kohonen::load(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
        throw std::runtime_error("No se pudo abrir el archivo para cargar el modelo.");

    file.read(reinterpret_cast<char *>(&rows_), sizeof(rows_));
    file.read(reinterpret_cast<char *>(&cols_), sizeof(cols_));
    file.read(reinterpret_cast<char *>(&input_size_), sizeof(input_size_));

    neurons_.resize(rows_, std::vector<SOMNeuron>(cols_));
    for (auto &row : neurons_)
    {
        for (auto &neuron : row)
        {
            neuron.weights.resize(input_size_);
            file.read(reinterpret_cast<char *>(neuron.weights.data()), input_size_ * sizeof(double));
        }
    }
}