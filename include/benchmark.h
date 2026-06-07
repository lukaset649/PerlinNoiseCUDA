#pragma once

#include <vector>

struct Resolution
{
    int width;
    int height;
};

void runBenchmark(int n, bool saveImg, bool saveCsv, bool warmup, const std::vector<Resolution>& resolutions, float scale, int octaves, float persistence, float lacunarity);
