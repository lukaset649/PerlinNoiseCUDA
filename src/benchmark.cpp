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
        vector<unsigned char> imageGPUOpt(res.width * res.height);

        double cpuTime = generateNoiseCPU(imageCPU.data(), res.width, res.height, scale, 5, 0.5f, 2.0f);
        double gpuTime = generateNoiseGPU(imageGPU.data(), res.width, res.height, scale, 5, 0.5f, 2.0f);
        double gpuOptTime = generateNoiseGPUOptimized(imageGPUOpt.data(), res.width, res.height, scale, 5, 0.5f, 2.0f);

        string tag = to_string(res.width) + "x" + to_string(res.height);
        savePGM(("output/" + tag + "_cpu.pgm").c_str(), imageCPU.data(), res.width, res.height);
        savePGM(("output/" + tag + "_gpu.pgm").c_str(), imageGPU.data(), res.width, res.height);
        savePGM(("output/" + tag + "_gpu_opt.pgm").c_str(), imageGPUOpt.data(), res.width, res.height);

        cout << "\n-----------------------------\n";
        cout << "Resolution: "<< res.width << "x" << res.height << '\n' << '\n';
        cout << "CPU time: "<< cpuTime << " s\n";
        cout << "GPU time: "<< gpuTime << " s\n";
        cout << "GPU opt time: " << gpuOptTime << " s\n";
        cout << "-----------------------------\n";
    }
}

void runBenchmarkIterations(int n, const vector<Resolution>& resolutions, float scale, int octaves, float persistence, float lacunarity)
{
    cout << fixed << setprecision(6);

    for (const auto& res : resolutions)
    {
        double cpuSum = 0.0;
        double gpuSum = 0.0;
        double gpuOptSum = 0.0;

        vector<unsigned char> imageCPU(res.width * res.height);
        vector<unsigned char> imageGPU(res.width * res.height);
        vector<unsigned char> imageGPUOpt(res.width * res.height);

        for (int i = 0; i < n; ++i)
        {
            cpuSum += generateNoiseCPU(imageCPU.data(), res.width, res.height, scale, 5, 0.5f, 2.0f);
            gpuSum += generateNoiseGPU(imageGPU.data(), res.width, res.height, scale, 5, 0.5f, 2.0f);
            gpuOptSum += generateNoiseGPUOptimized(imageGPUOpt.data(), res.width, res.height, scale, 5, 0.5f, 2.0f);
        }

        double cpuAvg = cpuSum / n;
        double gpuAvg = gpuSum / n;
        double gpuOptAvg = gpuOptSum / n;

        cout << "\n-----------------------------\n";
        cout << "Resolution: " << res.width << "x" << res.height << '\n';
        cout << "Iterations: " << n << "\n\n";
        cout << "Avg time:\n";
        cout << "CPU: " << cpuAvg << " s\n";
        cout << "GPU avg time: " << gpuAvg << " s\n";
        cout << "GPU opt avg time: " << gpuOptAvg << " s\n";
        cout << "-----------------------------\n";
    }
}
