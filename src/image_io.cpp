#include "../include/image_io.h"
#include <fstream>

void savePGM(const char* filename, const unsigned char* image, int width, int height)
{
	std::ofstream file(filename);

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
}
