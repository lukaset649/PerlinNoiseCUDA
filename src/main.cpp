#include "../include/benchmark.h"
#include <iostream>
#include <string>

using namespace std;

int main()
{
    string input;
    bool saveImg = true;
    bool saveCsv = true;
    bool warmup = true;
    int iterations = 1;

    float scale = 0.02f;
    int octaves = 5;
    float persistence = 0.5f;
    float lacunarity = 2.0f;

    vector<Resolution> resolutions =
    {
        {256, 256},
        {512, 512},
        {1024, 1024},
        {2048, 2048},
    };


    cout << "Save images? (Y/n): ";
    getline(cin, input);
    if (!input.empty())
        saveImg = (input[0] == 'y' || input[0] == 'Y');

    cout << "Save CSV? (Y/n): ";
    getline(cin, input);
    if (!input.empty())
        saveCsv = (input[0] == 'y' || input[0] == 'Y');

    cout << "Warmup? (Y/n): ";
    getline(cin, input);
    if (!input.empty())
        warmup = (input[0] == 'y' || input[0] == 'Y');

    cout << "Iterations [1]: ";
    getline(cin, input);
    if (!input.empty())
        iterations = stoi(input);

    runBenchmark(iterations, saveImg, saveCsv, warmup, resolutions, scale, octaves, persistence, lacunarity);

    return 0;
}
