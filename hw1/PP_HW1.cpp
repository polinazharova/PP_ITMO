#pragma GCC target("avx2")
#include <iostream>
#include <immintrin.h>
#include <chrono>

void arr_filling(float* arr, size_t n) {
    for (size_t i{ 0 }; i < n; i++) {
        arr[i] = rand() % 100;
    }
}

void arr_printing(float* x_arr, float* y_arr, size_t n) {
    std::cout << "FIRST:    SECOND: " << std::endl;
    for (size_t i{ 0 }; i < n; i++) {
        std::cout << x_arr[i] << "    " << y_arr[i] << std::endl;
    }
}


float vector(float* x_arr, float* y_arr, size_t n)
{
    float s = 0;
    for (size_t i = 0; i < n; i++)
        s += x_arr[i] * y_arr[i];
    return s;
}

float vector_sse(float* x_arr, float* y_arr, size_t n)
{
    float s = 0;
    float temp_arr[8];
    size_t i = 0;
    for (i; i + 8 < n; i += 8) {
        __m256 x = _mm256_loadu_ps(&x_arr[i]);
        __m256 y = _mm256_loadu_ps(&y_arr[i]);
        __m256 res = _mm256_mul_ps(x, y);
        __m256 temp = _mm256_hadd_ps(res, res); 
        temp = _mm256_hadd_ps(temp, temp);
        _mm256_storeu_ps(temp_arr, temp);
        s += temp_arr[0] + temp_arr[4];
    }
    for (; i < n; i++) {
        s += x_arr[i] * y_arr[i];
    }
    
    return s;
}

void square(float* init_arr, float* res_arr, size_t n) {
    for (size_t i{ 0 }; i < n; i++) {
        res_arr[i] = sqrt(init_arr[i]);
    }
}

void square_sse(float* init_arr, float* res_arr, size_t n) {
    size_t i = 0;
  
    for (; i + 8 < n; i += 8) {
        __m256 init = _mm256_load_ps(&init_arr[i]);
        __m256 res = _mm256_sqrt_ps(init);
        _mm256_store_ps(&res_arr[i], res);
    }

    for (; i < n; i++) {
        res_arr[i] = sqrt(init_arr[i]);
    }
}

int main() {
    std::cout << "N: ";
    size_t N; std::cin >> N;

    alignas(32) float* x = new float[N];
    alignas(32) float* y = new float[N];

    arr_filling(x, N);

    std::cout << "SSE SQUARE:" << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    square_sse(x, y, N);
    auto end_time = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    std::cout << "TIME: " << time.count() << std::endl;
  
    std::cout << "STANDART SQUARE:" << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    square(x, y, N);
    end_time = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    std::cout << "TIME: " << time.count() << std::endl;

    std::cout << "SSE VECTOR:" << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    std::cout << vector_sse(x, y, N) << std::endl;
    end_time = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    std::cout << "TIME: " << time.count() << std::endl;

    std::cout << "STANDART VECTOR:" << std::endl;
    start_time = std::chrono::high_resolution_clock::now();
    std::cout << vector(x, y, N) << std::endl;
    end_time = std::chrono::high_resolution_clock::now();
    time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
    std::cout << "TIME: " << time.count() << std::endl;

    delete[] x;
    delete[] y;

    return 0;
}