#include "visualization.h"
#include "training.h"

int main(int argc, char **argv)
{
    auto patterns = KohonenTraining::generateSimplePatterns();
    Kohonen3D *som = new Kohonen3D(3, 3, 3, patterns[0].size());
    KohonenVisualizer visualizer(som);
    visualizer.training_patterns = patterns;
    visualizer.initGL(argc, argv);
    visualizer.run();
    delete som;
    return 0;
}