#pragma once

#include <vector>

struct Resolution
{
    int width;
    int height;
};

void runBenchmark(const std::vector<Resolution>& resolutions, float scale, int octaves, float persistence, float lacunarity);
