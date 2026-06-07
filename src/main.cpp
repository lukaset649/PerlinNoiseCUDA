#include "../include/perlin.h"
#include "../include/image_io.h"
#include <chrono>
#include <iostream>
#include <vector>
#include <iomanip>

int main()
{
    constexpr int width = 1024;
    constexpr int height = 1024;
    constexpr float scale = 0.02f;

    std::vector<unsigned char> imageCPU(width * height);
    std::vector<unsigned char> imageGPU(width * height);

    ////CPU
    //auto cpuStart = std::chrono::high_resolution_clock::now();

    double cpuTime = generateNoiseCPU(imageCPU.data(), width, height, scale, 5, 0.5f, 2.0f);

    //auto cpuEnd = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> cpuElapsed =cpuEnd - cpuStart;

    //savePGM("output/noise_cpu.pgm", imageCPU.data(), width, height);

    ////GPU
    //auto gpuStart = std::chrono::high_resolution_clock::now();

    double gpuTime = generateNoiseGPU(imageGPU.data(), width, height, scale, 5, 0.5f, 2.0f);

    //auto gpuEnd = std::chrono::high_resolution_clock::now();
    //std::chrono::duration<double> gpuElapsed = gpuEnd - gpuStart;

    //savePGM("output/noise_gpu.pgm", imageGPU.data(), width, height);

    ////proste porównanie
    //int differences = 0;

    //for (int i = 0; i < width * height; i++)
    //{
    //    if (imageCPU[i] != imageGPU[i])
    //    {
    //        differences++;
    //    }
    //}

    ////wyniki
    //std::cout << "CPU execution time: "
    //    << cpuElapsed.count()
    //    << " s\n";

    //std::cout << "GPU execution time: "
    //    << gpuElapsed.count()
    //    << " s\n";

    //std::cout << "Different pixels: "
    //    << differences
    //    << '\n';

    std::cout << std::fixed << std::setprecision(9);

    std::cout << "CPU: " << cpuTime << " s\n";
    std::cout << "GPU: " << gpuTime << " s\n";

    return 0;
}
