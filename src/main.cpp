#include "../include/perlin.h"

#include <chrono>
#include <iostream>

int main()
{
    constexpr int width = 1024;
    constexpr int height = 1024;

    constexpr float scale = 0.02f;

    volatile float sum = 0.0f;

    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            sum += perlin(x * scale, y * scale);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;

    std::cout << "CPU execution time: "
        << elapsed.count()
        << " s\n";

    return 0;
}