#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <cstring>
#include <iostream>
#include "../include/perlin.h"
#include "../include/permutation.h"

// d_permutation zdefiniowana w perlin_gpu.cu i współdzielona przez linkowanie
extern __constant__ int d_permutation[512];

// ============================================================
//  WERSJA ZOPTYMALIZOWANA
//
//  Zastosowane optymalizacje względem perlin_gpu.cu:
//
//  1. __forceinline__
//     Eliminuje narzut wywołania funkcji – kompilator NVCC
//     wstawia ciało funkcji w miejscu wywołania, zmniejszając
//     liczbę instrukcji i eliminując zarządzanie stosem wątku.
//
//  2. __float2int_rd (hardware floor + cast)
//     Sprzętowa instrukcja zaokrąglania w dół (round down).
//     Zastępuje sekwencję floorf() + rzutowanie na int,
//     wykonując obie operacje w jednym cyklu zegarowym.
//
//  3. __fmaf_rn (Fused Multiply-Add)
//     Instrukcja FMA łączy mnożenie i dodawanie w jednej
//     operacji sprzętowej (a*b + c). Poprawia throughput
//     jednostek FP32 oraz dokładność numeryczną (jedno
//     zaokrąglenie zamiast dwóch).
//
//  4. Shared memory na tablicę permutacji
//     Każdy blok ładuje 512 int-ów z __constant__ do __shared__
//     jednorazowo na początku kernela. Kolejne odczyty trafiają
//     w pamięć współdzieloną SM (~1 cykl latencji) zamiast
//     w cache L1/L2 (~20-80 cykli). Przynosi największy zysk
//     przy wielu oktawach FBM (wiele odczytów na piksel).
//
//  5. Pinned memory (cudaMallocHost) po stronie hosta
//     Bufor wyjściowy alokowany jako page-locked (przypięty)
//     umożliwia bezpośredni transfer DMA GPU→CPU z pominięciem
//     tymczasowego bufora systemu operacyjnego. Skraca czas
//     cudaMemcpy, szczególnie widoczne przy rozdzielczościach
//     2048×2048 i wyższych.
//
//  6. Rozmiar bloku 32×8 zamiast 16×16
//     Szerokość bloku = 32 odpowiada rozmiarowi warpa CUDA.
//     Zapis output[y*width+x] dla x=0..31 trafia do kolejnych
//     adresów pamięci globalnej → pełna koalescencja (jeden
//     transaction 128B zamiast wielu). Łączna liczba wątków
//     (256) pozostaje optymalna dla wypełnienia SM.
// ============================================================

__device__ __forceinline__ float fade_opt(float t)
{
    // Wzór: t^3 * (t*(t*6 - 15) + 10)
    // Wewnętrzne FMA: t*6 + (-15), zewnętrzne FMA: t*(wynik) + 10
    return t * t * t * __fmaf_rn(t, __fmaf_rn(t, 6.0f, -15.0f), 10.0f);
}

__device__ __forceinline__ float lerp_opt(float a, float b, float t)
{
    // a + t*(b-a)  przepisane jako FMA: t*(b-a) + a
    return __fmaf_rn(t, b - a, a);
}

__device__ __forceinline__ float grad_opt(int hash, float x, float y)
{
    int h   = hash & 7;
    float u = h < 4 ? x : y;
    float v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

__device__ __forceinline__ float perlin_opt(float x, float y, const int* __restrict__ s_perm)
{
    // __float2int_rd: sprzętowy floor + konwersja do int w jednej instrukcji
    int X = __float2int_rd(x) & 255;
    int Y = __float2int_rd(y) & 255;

    // Część ułamkowa: x - floor(x)
    x -= __int2float_rn(__float2int_rd(x));
    y -= __int2float_rn(__float2int_rd(y));

    float u = fade_opt(x);
    float v = fade_opt(y);

    // Odczyty z shared memory – latencja ~1 cykl zamiast ~80 (DRAM)
    int aa = s_perm[s_perm[X]     + Y    ];
    int ab = s_perm[s_perm[X]     + Y + 1];
    int ba = s_perm[s_perm[X + 1] + Y    ];
    int bb = s_perm[s_perm[X + 1] + Y + 1];

    return lerp_opt(
        lerp_opt(grad_opt(aa, x,        y        ), grad_opt(ba, x - 1.0f, y        ), u),
        lerp_opt(grad_opt(ab, x,        y - 1.0f ), grad_opt(bb, x - 1.0f, y - 1.0f), u),
        v);
}

__device__ __forceinline__ float fbm_opt(
    float x, float y,
    int octaves, float persistence, float lacunarity,
    const int* __restrict__ s_perm)
{
    float amplitude = 1.0f;
    float frequency = 1.0f;
    float sum       = 0.0f;
    float maxValue  = 0.0f;

    for (int i = 0; i < octaves; i++)
    {
        sum      += perlin_opt(x * frequency, y * frequency, s_perm) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= lacunarity;
    }

    return sum / maxValue;
}

__global__ void noiseKernelOptimized(
    unsigned char* __restrict__ output,
    int width, int height,
    float scale, int octaves,
    float persistence, float lacunarity)
{
    // Krok 1: załaduj tablicę permutacji do shared memory
    // Każdy z 256 wątków ładuje 2 elementy (512 / 256 = 2 iteracje)
    __shared__ int s_perm[512];

    int tid       = threadIdx.y * blockDim.x + threadIdx.x;
    int blockSize = blockDim.x * blockDim.y;

    for (int i = tid; i < 512; i += blockSize)
        s_perm[i] = d_permutation[i];

    // Bariera synchronizacji – wszystkie wątki muszą skończyć ładowanie
    // zanim którykolwiek zacznie używać s_perm
    __syncthreads();

    // Krok 2: oblicz wartość szumu dla piksela (x, y)
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x >= width || y >= height)
        return;

    float value = fbm_opt(x * scale, y * scale, octaves, persistence, lacunarity, s_perm);

    // (value + 1.0) * 0.5 jako FMA: value * 0.5 + 0.5
    value = __fmaf_rn(value, 0.5f, 0.5f);

    int gray = __float2int_rn(value * 255.0f);
    gray = max(0, min(255, gray));

    output[y * width + x] = static_cast<unsigned char>(gray);
}

double generateNoiseGPUOptimized(
    unsigned char* output,
    int width, int height,
    float scale, int octaves,
    float persistence, float lacunarity)
{
    // initPermutationGPU() zostało już wywołane przez generateNoiseGPU()
    // w benchmarku; wywołaj je tutaj na wypadek samodzielnego użycia
    initPermutationGPU();

    size_t bytes = (size_t)width * height * sizeof(unsigned char);

    // Pinned memory: bufor page-locked po stronie hosta
    // umożliwia DMA transfer bez pośredniczącego bufora OS
    unsigned char* h_pinned;
    cudaMallocHost(&h_pinned, bytes);

    unsigned char* d_output;
    cudaMalloc(&d_output, bytes);

    // 32 (warp) × 8 = 256 wątków na blok
    // Szerokość = 32 → koalescencja zapisu do pamięci globalnej
    dim3 block(32, 8);
    dim3 grid((width  + block.x - 1) / block.x,
              (height + block.y - 1) / block.y);

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    noiseKernelOptimized<<<grid, block>>>(
        d_output, width, height,
        scale, octaves, persistence, lacunarity);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);

    float elapsedMs = 0.0f;
    cudaEventElapsedTime(&elapsedMs, start, stop);

    cudaError_t err = cudaGetLastError();
    if (err != cudaSuccess)
        std::cout << "Kernel (opt) error: " << cudaGetErrorString(err) << '\n';

    cudaMemcpy(h_pinned, d_output, bytes, cudaMemcpyDeviceToHost);
    memcpy(output, h_pinned, bytes);

    cudaFreeHost(h_pinned);
    cudaFree(d_output);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return static_cast<double>(elapsedMs) / 1000.0;
}
