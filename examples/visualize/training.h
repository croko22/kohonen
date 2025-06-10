#pragma once

#include <vector>
#include "../../src/kohonen3d.h"

namespace KohonenTraining
{
    std::vector<std::vector<double>> generateSimplePatterns();
    void trainNetwork(Kohonen3D *som, const std::vector<std::vector<double>> &patterns);
}