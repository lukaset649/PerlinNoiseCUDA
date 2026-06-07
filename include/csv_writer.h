#pragma once

#include <string>

void appendBenchmarkResult(const std::string& filename, int width, int height, const std::string& implementation, int iteration, double time);

void createBenchmarkCsv(const std::string& filename);
