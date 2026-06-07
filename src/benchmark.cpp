#include "../include/benchmark.h"
#include "../include/perlin.h"
#include "../include/image_io.h"
#include "../include/csv_writer.h"
#include <iostream>
#include <vector>
#include <iomanip>
#include <string>

using namespace std;

void runBenchmark(
    int n,
    bool saveImg,
    bool saveCsv,
    bool warmup,
    const vector<Resolution>& resolutions,
    float scale,
    int octaves,
    float persistence,
    float lacunarity
){
    cout << fixed << setprecision(6);

    if (saveCsv)
    {
        createBenchmarkCsv("output/benchmark.csv");
    }

    int warmupRes = 255;
    vector<unsigned char> warmupV(warmupRes * warmupRes);

    if (warmup)
    {
        for (int i = 0; i < 3; ++i)
        {
            generateNoiseCPU(warmupV.data(), warmupRes, warmupRes, scale, octaves, persistence, lacunarity);
            generateNoiseGPU(warmupV.data(), warmupRes, warmupRes, scale, octaves, persistence, lacunarity);
            generateNoiseGPUOptimized(warmupV.data(), warmupRes, warmupRes, scale, octaves, persistence, lacunarity);
        }
    }

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
            cpuSum += generateNoiseCPU(imageCPU.data(), res.width, res.height, scale, octaves, persistence, lacunarity);
            gpuSum += generateNoiseGPU(imageGPU.data(), res.width, res.height, scale, octaves, persistence, lacunarity);
            gpuOptSum += generateNoiseGPUOptimized(imageGPUOpt.data(), res.width, res.height, scale, octaves, persistence, lacunarity);

            if (saveCsv)
            {
                appendBenchmarkResult("output/benchmark.csv", res.width, res.height, "CPU", i + 1, cpuSum);
                appendBenchmarkResult("output/benchmark.csv", res.width, res.height, "GPU", i + 1, gpuSum);
                appendBenchmarkResult("output/benchmark.csv", res.width, res.height, "GPU_OPT", i + 1, gpuOptSum);
            }
        }

        if (saveImg)
        {
            string tag = to_string(res.width) + "x" + to_string(res.height);
            savePGM(("output/" + tag + "_cpu.pgm").c_str(), imageCPU.data(), res.width, res.height);
            savePGM(("output/" + tag + "_gpu.pgm").c_str(), imageGPU.data(), res.width, res.height);
            savePGM(("output/" + tag + "_gpu_opt.pgm").c_str(), imageGPUOpt.data(), res.width, res.height);
        }

        double cpuAvg = cpuSum / n;
        double gpuAvg = gpuSum / n;
        double gpuOptAvg = gpuOptSum / n;

        cout << "\n-----------------------------\n";
        cout << "Resolution: " << res.width << "x" << res.height << '\n';
        cout << "Iterations: " << n << "\n\n";
        cout << "Avg time:\n";
        cout << "CPU: " << cpuAvg << " s\n";
        cout << "GPU: " << gpuAvg << " s\n";
        cout << "GPU_OPT: " << gpuOptAvg << " s\n";
        cout << "-----------------------------\n";
    }
}
