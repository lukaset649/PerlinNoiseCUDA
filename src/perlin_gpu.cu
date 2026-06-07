#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <cmath>
#include <iostream>
#include "../include/perlin.h"
#include "../include/permutation.h"

__constant__ int d_permutation[512];

void initPermutationGPU()
{
    int h_permutation[512];

    for (int i = 0; i < 256; i++)
    {
        h_permutation[i] = basePermutation[i];
        h_permutation[i + 256] = basePermutation[i];
    }

    cudaMemcpyToSymbol(d_permutation,h_permutation,sizeof(h_permutation));
}

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

__global__ void noiseKernel(unsigned char* output, int width, int height, float scale, int octaves, float persistence, float lacunarity)
{
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    //zabezpieczenie przed wyjœciem pixeli w bloku poza rozmiar obrazu
    if (x >= width || y >= height)
    {
        return;
    }
    
    float value = fbm_device(x * scale, y * scale, octaves, persistence, lacunarity);
    value = (value + 1.0f) * 0.5f;

    int gray = (int)(value * 255.0f);
    gray = max(0, min(255, gray));

    output[y * width + x] = static_cast<unsigned char>(gray);
}

void generateNoiseGPU(unsigned char* output, int width, int height, float scale, int octaves, float persistence, float lacunarity)
{
    initPermutationGPU();

    size_t bytes = width * height * sizeof(unsigned char);
    unsigned char* d_output;

    cudaMalloc(&d_output, bytes);

    dim3 block(16, 16);
    dim3 grid((width + block.x - 1) / block.x, (height + block.y - 1) / block.y);

    noiseKernel<<<grid, block>>>(d_output, width, height, scale, octaves, persistence, lacunarity);

    cudaDeviceSynchronize();

    cudaError_t err = cudaGetLastError();

    if (err != cudaSuccess)
    {
        std::cout << "Kernel error: "
            << cudaGetErrorString(err)
            << '\n';
    }

    cudaMemcpy(output, d_output, bytes, cudaMemcpyDeviceToHost);

    cudaFree(d_output);
}
