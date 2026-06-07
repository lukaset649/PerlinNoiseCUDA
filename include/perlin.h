#pragma once

float perlin(float x, float y);

float fbm(float x, float y, int octaves, float persistence, float lacunarity);

void initPermutationGPU();

void generateNoiseGPU(unsigned char* output, int width, int height,float scale, int octaves, float persistence, float lacunarity);
