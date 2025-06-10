#include "training.h"

std::vector<std::vector<double>> KohonenTraining::generateSimplePatterns()
{
    std::vector<std::vector<double>> patterns;
    patterns.push_back({1.0, 0.0, 0.0});
    patterns.push_back({0.0, 1.0, 0.0});
    patterns.push_back({0.0, 0.0, 1.0});
    patterns.push_back({1.0, 1.0, 0.0});
    patterns.push_back({0.5, 0.5, 0.5});
    return patterns;
}

void KohonenTraining::trainNetwork(Kohonen3D *som, const std::vector<std::vector<double>> &patterns)
{
    som->train(patterns, 1, 0.1);
}