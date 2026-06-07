#include "../include/benchmark.h"
#include "../include/perlin.h"
#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

void runBenchmark(const std::vector<Resolution>& resolutions, float scale, int octaves, float persistence, float lacunarity)
{
    std::cout << std::fixed << std::setprecision(6);

    for (const auto& res : resolutions)
    {
        std::vector<unsigned char> imageCPU(res.width * res.height);
        std::vector<unsigned char> imageGPU(res.width * res.height);

        double cpuTime = generateNoiseCPU(imageCPU.data(), res.width, res.height, scale, 5, 0.5f, 2.0f);
        double gpuTime = generateNoiseGPU(imageGPU.data(), res.width, res.height, scale, 5, 0.5f, 2.0f);


        cout << "Resolution: "<< res.width << "x" << res.height << '\n';
        cout << "CPU time: "<< cpuTime << " s\n";
        cout << "GPU time: "<< gpuTime << " s\n";
        cout << "-----------------------------\n";
    }
}
