#include "../include/perlin.h"
#include <cmath>
#include "../include/permutation.h"

static bool initialized = false;
static int permutationTable[512];

static void initPermutation()
{
    if (initialized)
    {
		return;
    }

    for (int i = 0; i < 256; i++)
    {
        permutationTable[i] = basePermutation[i];
        permutationTable[256 + i] = basePermutation[i];
    }

	initialized = true;
}

static float fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

static float lerp(float a, float b, float t)
{
    return a + t * (b - a);
}

static float grad(int hash, float x, float y)
{
	int h = hash & 7;
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;

    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

float perlin(float x, float y)
{
    initPermutation();

    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    x -= floor(x);
    y -= floor(y);

    float u = fade(x);
    float v = fade(y);

    int aa = permutationTable[permutationTable[X] + Y];
    int ab = permutationTable[permutationTable[X] + Y + 1];
    int ba = permutationTable[permutationTable[X + 1] + Y];
    int bb = permutationTable[permutationTable[X + 1] + Y + 1];

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

float fbm(float x, float y, int octaves, float persistence, float lacunarity)
{
    float amplitude = 1.0f;
    float frequency = 1.0f;

    float sum = 0.0f;
    float maxValue = 0.0f;

    for (int i = 0; i < octaves; i++)
    {
        sum += perlin(x * frequency, y * frequency) * amplitude;

        maxValue += amplitude;

        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return sum / maxValue;
}

void generateNoiseCPU(unsigned char* output, int width, int height, float scale, int octaves, float persistence, float lacunarity)
{
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float value = fbm(x * scale, y * scale, octaves, persistence, lacunarity);

            value = (value + 1.0f) * 0.5f;

            int gray = static_cast<int>(value * 255.0f);


            //przyciêcie dla skali szaroœci 0-255
            if (gray < 0) {
                gray = 0;
            }

            if (gray > 255) {
                gray = 255;
            }

            output[y * width + x] = static_cast<unsigned char>(gray);
        }
    }
}
