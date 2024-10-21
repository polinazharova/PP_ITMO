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
    std::mt19937 gen(static_cast<unsigned int>(GetCurrentThreadId()));
    std::uniform_int_distribution<> dis(0, 99);

    ThreadParamsForMatrix* params = static_cast<ThreadParamsForMatrix*>(lpParam);
    long long mult = 1;
    for (size_t i{ 0 }; i < params->str_size; i++) {
        (params->arr_str)[i] = dis(gen);
        if ((i + 1) % 2 != 0) {
            mult *= (params->arr_str)[i];
        }
    }
    return static_cast<DWORD>(mult);
}

void str_printing(ThreadParamsForMatrix* params) {
    std::cout << "MULTIPLICATION: " << params->str_odd_mult << std::endl;
    for (size_t i{ 0 }; i < params->str_size; i++) {
        std::cout << (params->arr_str)[i] << "|";
    }
    std::cout << std::endl << std::endl;
}

void matrix_deleting(int** matrix, size_t n) {
    for (size_t i{ 0 }; i < n; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

int main()
{
    std::cout << "n and m: ";
    size_t n, m; std::cin >> n >> m;

    int** matrix = new int*[n];
    ThreadParamsForMatrix* params = new ThreadParamsForMatrix{NULL, m, NULL};
  
    for (int i{ 0 }; i < n; i++) {
        matrix[i] = new int[m];
        params->arr_str = matrix[i];
        HANDLE thread = CreateThread(NULL, 0, str_generating, params, 0, NULL);
        if (thread) {
            WaitForSingleObject(thread, INFINITE);
            DWORD exitCode;
            if (GetExitCodeThread(thread, &exitCode)) {
                params->str_odd_mult = static_cast<long long>(exitCode);
            }
            else {
                std::cerr << "Failed to get exit code." << std::endl;
                return -1;
            }
            CloseHandle(thread);
        }
        else {
            std::cerr << "Failed to create thread." << std::endl;
            return -1;
        }
        str_printing(params);
    }

    matrix_deleting(matrix, n);
    delete params;

    return 0;
}

