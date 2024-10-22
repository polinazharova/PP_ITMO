#include <iostream>
#include <windows.h>
#include <random>


struct ThreadParamsForMatrix
{
    int* arr_str;
    size_t str_size;
    long long str_odd_mult;
};

DWORD WINAPI str_generating(LPVOID lpParam) {
    ThreadParamsForMatrix* params = static_cast<ThreadParamsForMatrix*>(lpParam);
    long long mult = 1;
    std::mt19937 gen(static_cast<unsigned int>(GetCurrentThreadId()));
    std::uniform_int_distribution<> dis(0, 99);
    for (size_t i{ 0 }; i < params->str_size; i++) {
        (params->arr_str)[i] = dis(gen);
        if ((i + 1) % 2 != 0) {
            mult *= (params->arr_str)[i];
        }
    }
    if (params->str_size <= 2) {
        params->str_odd_mult = 0;
    }
    else {
        params->str_odd_mult = mult;
    }

    return 0;
}

void str_printing(ThreadParamsForMatrix* params) {
    std::cout << "MULTIPLICATION: " << params->str_odd_mult << std::endl;
    for (size_t i{ 0 }; i < params->str_size; i++) {
        std::cout << (params->arr_str)[i] << "|";
    }
    std::cout << std::endl << std::endl;
}


void params_matrix_deleting(ThreadParamsForMatrix** params, int** matrix, size_t n) {
    for (size_t i{ 0 }; i < n; i++) {
        delete[] params[i]->arr_str;
        delete[] params[i];
    }
    delete[] params;
    delete[] matrix;
}

int main()
{
    std::cout << "n and m: ";
    size_t n, m; std::cin >> n >> m;

    int** matrix = new int*[n];
    ThreadParamsForMatrix** params = new ThreadParamsForMatrix*[n];
    HANDLE* threads = new HANDLE[n];
  
    for (size_t i{ 0 }; i < n; i++) {
        params[i] = new ThreadParamsForMatrix{ nullptr, m, 0 };
        params[i]->arr_str = new int[m];
        threads[i] = CreateThread(NULL, 0, str_generating, static_cast<LPVOID>(params[i]), 0, NULL);
        if (!threads[i]) {
            std::cerr << "Failed to create a thread." << std::endl;
            return -1;
        }
    }

    WaitForMultipleObjects(n, threads, TRUE, INFINITE);

    for (size_t i{ 0 }; i < n; i++) {
        CloseHandle(threads[i]);
        str_printing(params[i]);
    }

    params_matrix_deleting(params, matrix, n);
    delete[] threads;

    return 0;
}

