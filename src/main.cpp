#include "../include/perlin.h"
#include "../include/benchmark.h"
#include "../include/image_io.h"
#include <chrono>
#include <iostream>
#include <vector>
#include <iomanip>

int main()
{
    std::vector<Resolution> resolutions =
    {
        {256, 256},
        {512, 512},
        {1024, 1024},
        {2048, 2048},
        //{4096, 4096}
    };

    runBenchmark(resolutions, 0.02f, 5, 0.5f, 2.0f);

    return 0;
}
