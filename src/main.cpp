#include "../include/perlin.h"
#include "../include/benchmark.h"
#include "../include/image_io.h"
#include <chrono>
#include <iostream>
#include <vector>
#include <iomanip>

int main()
{
    bool saveImg = true;
    bool saveSvg = true;
    int iterations = 2;

    float scale = 0.02f;
    int octaves = 5;
    float persistence = 0.5f;
    float lacunarity = 2.0f;

    std::vector<Resolution> resolutions =
    {
        {256, 256},
        {512, 512},
        {1024, 1024},
        {2048, 2048},
        //{4096, 4096}
    };

    runBenchmark(iterations, saveImg, saveSvg, resolutions, scale, octaves, persistence, lacunarity);

    return 0;
}
