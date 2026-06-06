#include "../include/perlin.h"

#include <chrono>
#include <fstream>
#include <iostream>

int main()
{
    constexpr int width = 1024;
    constexpr int height = 1024;

    constexpr float scale = 0.02f;

    auto start = std::chrono::high_resolution_clock::now();

    std::ofstream file("output/noise_cpu.pgm");

    file << "P2\n";
    file << width << " " << height << "\n";
    file << "255\n";

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float value = perlin(x * scale, y * scale);

            value = (value + 1.0f) * 0.5f;

            int gray = static_cast<int>(value * 255.0f);

            if (gray < 0) gray = 0;
            if (gray > 255) gray = 255;

            file << gray << ' ';
        }

        file << '\n';
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    std::cout << "CPU execution time: "
        << elapsed.count()
        << " s\n";

    return 0;
}