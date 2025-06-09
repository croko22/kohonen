
#pragma once

#include <vector>
#include <string>
#include <random>

struct SOMNeuron
{
    std::vector<double> weights;
};

class Kohonen
{
public:
    Kohonen(int rows, int cols, int input_size);

    void train(const std::vector<std::vector<double>> &patterns, int epochs, double initial_learning_rate = 0.1);

    std::pair<int, int> predict(const std::vector<double> &pattern) const;

    void save(const std::string &filename) const;

    void load(const std::string &filename);

    const std::vector<std::vector<double>> &getTrainedPatterns() const { return trained_patterns_; }

private:
    int rows_;
    int cols_;
    int input_size_;
    std::vector<std::vector<SOMNeuron>> neurons_;

    mutable std::mt19937 rng_;
    std::uniform_real_distribution<double> weight_dist_;

    std::vector<std::vector<double>> trained_patterns_;

    void initializeWeights();

    std::pair<int, int> findBMU(const std::vector<double> &pattern) const;

    void updateWeights(const std::vector<double> &pattern, const std::pair<int, int> &bmu,
                       double learning_rate, double radius);
};