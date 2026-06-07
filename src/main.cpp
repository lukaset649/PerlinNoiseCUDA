#include "../include/perlin.h"
#include "../include/image_io.h"
#include <chrono>
#include <iostream>
#include <vector>

int main()
{
    constexpr int width = 1024;
    constexpr int height = 1024;

    constexpr float scale = 0.02f;

    std::vector<unsigned char> image(width * height);

    auto start = std::chrono::high_resolution_clock::now();

    generateNoiseCPU(image.data(), width, height, scale, 5, 0.5f, 2.0f);

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    savePGM("output/noise_cpu.pgm", image.data(), width, height);

    std::cout << "CPU execution time: "
        << elapsed.count()
        << " s\n";

    return 0;
}