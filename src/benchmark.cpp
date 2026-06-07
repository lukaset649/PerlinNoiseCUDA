#include "../include/benchmark.h"
#include "../include/perlin.h"
#include "../include/image_io.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

using namespace std;

void runBenchmark(const vector<Resolution>& resolutions, float scale, int octaves, float persistence, float lacunarity)
{
    cout << fixed << setprecision(6);

    for (const auto& res : resolutions)
    {
        vector<unsigned char> imageCPU(res.width * res.height);
        vector<unsigned char> imageGPU(res.width * res.height);

        double cpuTime = generateNoiseCPU(imageCPU.data(), res.width, res.height, scale, 5, 0.5f, 2.0f);
        double gpuTime = generateNoiseGPU(imageGPU.data(), res.width, res.height, scale, 5, 0.5f, 2.0f);

        string cpuFilename = "output/" + to_string(res.width) + "x" + to_string(res.height) + "_cpu.pgm";
        string gpuFilename = "output/" + to_string(res.width) + "x" + to_string(res.height) + "_gpu.pgm";

        savePGM(cpuFilename.c_str(), imageCPU.data(), res.width, res.height);
        savePGM(gpuFilename.c_str(), imageGPU.data(), res.width, res.height);

        cout << "Resolution: "<< res.width << "x" << res.height << '\n' << '\n';
        cout << "CPU time: "<< cpuTime << " s\n";
        cout << "GPU time: "<< gpuTime << " s\n";
        cout << "-----------------------------\n";
    }
}
