#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <cmath>
#include <iostream>

#include "../include/perlin.h"

#define WIDTH 1024
#define HEIGHT 1024

__device__ __constant__ int d_permutation[512];

__device__ float fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

__device__ float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

__device__ float grad(int hash, float x, float y)
{
    int h = hash & 7;
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;

    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

__device__ float perlin_device(float x, float y)
{
    int X = ((int)floorf(x)) & 255;
    int Y = ((int)floorf(y)) & 255;

    x -= floorf(x);
    y -= floorf(y);

    float u = fade(x);
    float v = fade(y);

    int aa = d_permutation[d_permutation[X] + Y];
    int ab = d_permutation[d_permutation[X] + Y + 1];
    int ba = d_permutation[d_permutation[X + 1] + Y];
    int bb = d_permutation[d_permutation[X + 1] + Y + 1];

    float result =
        lerp(
            lerp(
                grad(aa, x, y),
                grad(ba, x - 1.0f, y),
                u),
            lerp(
                grad(ab, x, y - 1.0f),
                grad(bb, x - 1.0f, y - 1.0f),
                u),
            v);

    return result;
}

__device__ float fbm_device(float x, float y, int octaves, float persistence, float lacunarity)
{
    float amplitude = 1.0f;
    float frequency = 1.0f;

    float sum = 0.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < octaves; i++)
    {
        sum += perlin_device(x * frequency, y * frequency) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return sum / maxValue;
}
