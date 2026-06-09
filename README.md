# PerlinNoiseCUDA

## Project Overview

PerlinNoiseCUDA is a project focused on the implementation and performance analysis of Perlin Noise generation using both CPU and GPU computing techniques.

The project includes three different implementations:

* **CPU implementation** written in C++,
* **Basic GPU implementation** using NVIDIA CUDA,
* **Optimized GPU implementation** utilizing CUDA-specific optimizations.

The main objective of the project is to compare the performance of sequential and parallel approaches to procedural noise generation and evaluate the impact of low-level GPU optimizations on execution time.

## Features

* 2D Perlin Noise generation
* Fractal Brownian Motion (fBm)
* CPU implementation in C++
* GPU implementation using CUDA
* Optimized CUDA implementation
* Automated benchmarking
* Export of benchmark results to CSV files
* Generation and saving of grayscale noise images in PGM format
* Performance comparison across multiple image resolutions

## Tested Resolutions

The benchmarking framework evaluates all implementations using the following image resolutions:

* 256 × 256
* 512 × 512
* 1024 × 1024
* 2048 × 2048

The number of octaves can be varied to analyze the impact of algorithm complexity on execution time.

## Technologies

* C++17
* NVIDIA CUDA
* Visual Studio 2022
* CUDA Toolkit 12.x

## Project Structure

```text
docs/       - project documentation
include/    - header files
src/        - source code
output/     - generated images and benchmark results
```

## Building the Project

### Requirements

* Visual Studio 2022
* NVIDIA CUDA Toolkit 12.x or newer
* CUDA-capable NVIDIA GPU

### Build Instructions

1. Open `PerlinNoiseCUDA.sln` in Visual Studio.
2. Select the `Release x64` configuration.
3. Build the solution.
4. Run the application.

## Output

The application can generate:

* Perlin Noise images (`.pgm`)
* Benchmark results (`.csv`)
* Performance statistics displayed in the console

Generated files are stored in the `output` directory.

## Documentation

The complete project documentation is available in:

```text
docs/dokumentacja.pdf (polish)
```

