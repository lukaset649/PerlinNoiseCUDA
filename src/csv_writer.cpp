#include "../include/csv_writer.h"
#include <fstream>
#include <filesystem>
#include <iomanip>

using namespace std;

void createBenchmarkCsv(const string& filename)
{
    ofstream file(filename);

    file << "width;height;implementation;iteration;time\n";
}

void appendBenchmarkResult(const string& filename, int width, int height, const string& implementation,int iteration, double time)
{
    ofstream file(filename, ios::app);

    file << fixed << setprecision(9);

    file << width << ";"
        << height << ";"
        << implementation << ";"
        << iteration << ";"
        << time << "\n";
}