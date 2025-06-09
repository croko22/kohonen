#pragma once

#include <vector>
#include <string>
#include <random>
#include <cmath>

struct SOMNeuron3D
{
    std::vector<double> weights;
};

class Kohonen3D
{
public:
    Kohonen3D(int x_dim, int y_dim, int z_dim, int input_size);

    void train(const std::vector<std::vector<double>> &patterns, int epochs,
               double initial_learning_rate = 0.1);

    std::tuple<int, int, int> predict(const std::vector<double> &pattern) const;

    void save(const std::string &filename) const;
    void load(const std::string &filename);

    const std::vector<std::vector<double>> &getTrainedPatterns() const { return trained_patterns_; }

    // private:
    int x_dim_;
    int y_dim_;
    int z_dim_;
    int input_size_;
    std::vector<std::vector<std::vector<SOMNeuron3D>>> neurons_;

    mutable std::mt19937 rng_;
    std::uniform_real_distribution<double> weight_dist_;

    std::vector<std::vector<double>> trained_patterns_;

    void initializeWeights();
    std::tuple<int, int, int> findBMU(const std::vector<double> &pattern) const;
    void updateWeights(const std::vector<double> &pattern,
                       const std::tuple<int, int, int> &bmu,
                       double learning_rate, double radius);
    double distance3D(int x1, int y1, int z1, int x2, int y2, int z2) const;
};