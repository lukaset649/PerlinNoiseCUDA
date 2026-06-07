#include "../include/perlin.h"
#include <chrono>
#include <fstream>
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

    //TODO: wydzieliæ zapis do pliku 
    std::ofstream file("output/noise_cpu.pgm");

    file << "P2\n";
    file << width << " " << height << "\n";
    file << "255\n";

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            file << static_cast<int>(
                image[y * width + x])
                << ' ';
        }

        file << '\n';
    }

    std::cout << "CPU execution time: "
        << elapsed.count()
        << " s\n";

    return 0;
}