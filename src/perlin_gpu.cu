#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <cmath>
#include <iostream>

#include "../include/perlin.h"

#define WIDTH 1024
#define HEIGHT 1024

__device__ __constant__ int d_permutation[512];
