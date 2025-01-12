#pragma GCC target("avx2")
#include <iostream>
#include <immintrin.h>
#include <chrono>
#include <vector>
#include <omp.h>

const size_t MAXSIZE1 = 8192;
const size_t MAXSIZE2 = 16384;
const size_t MAXSIZE3 = 32768;

double timeCounter(int (*func)(const std::vector<int>& vec), const std::vector<int>& vec) {
    double start = omp_get_wtime();
    func(vec);
    double finish = omp_get_wtime();

    return finish - start;
}

int noPar(const std::vector<int>& vec) {
    int sum = 0;
    for (int i{ 0 }; i < vec.size(); i++) {
        sum += vec[i];
    }
    return sum;
}

int SSE(const std::vector<int>& vec) {
    int sum = 0;
    __m256i sum_vec = _mm256_setzero_si256();
    size_t i = 0;
    for (i; i + 8 < vec.size(); i += 8) {
        __m256i x = _mm256_loadu_si256((__m256i*)&vec[i]);
        sum_vec = _mm256_add_epi32(sum_vec, x);
    }

    int sum_tmp[8];
    _mm256_storeu_si256((__m256i*)sum_tmp, sum_vec);

    for (int j = 0; j < 8; ++j) {
        sum += sum_tmp[j];
    }
    for (; i < vec.size(); i++) {
        sum += vec[i];
    }

    return sum;
}

int Cascade(const std::vector<int>& vec) {
    int n = vec.size();
    std::vector<int> newVec(vec.size());
    memcpy(newVec.data(), vec.data(), vec.size() * sizeof(int));

    while (n > 1) {
        int new_n = (n + 1) / 2;
        for (int j = 0; j < new_n; ++j) {
            newVec[j] = newVec[2 * j] + (2 * j + 1 < n ? newVec[2 * j + 1] : 0);
        }
        n = new_n;
    }

    return newVec[0];
}

int forOpenMP(const std::vector<int>& vec) {
    int sum = 0;
#pragma omp parallel for reduction(+:sum)
    for (int i{0}; i < vec.size(); i++) {
        sum += vec[i];
    }

    return sum;
}

int sectionsOpenMP(const std::vector<int>& vec) {
    int sum1 = 0;
    int sum2 = 0;

#pragma omp parallel sections 
    {
        for (int i{ 0 }; i < vec.size() / 2; i++) {
            sum1 += vec[i];
        }
#pragma omp section
        for (int i = vec.size() / 2; i < vec.size(); i++) {
            sum2 += vec[i];
        }
    }

    return sum1 + sum2;
}

int main()
{
    std::vector<int> vec1;
    for (int i{ 1 }; i <= MAXSIZE1; i++) {
        vec1.push_back(i);
    }
    std::vector<int> vec2;
    for (int i{ 1 }; i <= MAXSIZE2; i++) {
        vec2.push_back(i);
    }
    std::vector<int> vec3;
    for (int i{ 1 }; i <= MAXSIZE3; i++) {
        vec3.push_back(i);
    }

    std::cout << "---------------MAXSIZE = " << MAXSIZE1 << "---------------" << std::endl;
    std::cout <<"NoPar sum: " << noPar(vec1) <<" time: " << timeCounter(noPar, vec1) << std::endl;
    std::cout << "SSE sum: " << noPar(vec1) << " time: " << timeCounter(SSE, vec1) << std::endl;
    std::cout << "Cascade sum: " << noPar(vec1) << " time: " << timeCounter(Cascade, vec1) << std::endl;
    std::cout << "forOpenMP sum: " << noPar(vec1) << " time: " << timeCounter(forOpenMP, vec1) << std::endl;
    std::cout << "Sections OpenMP sum: " << noPar(vec1) << " time: " << timeCounter(sectionsOpenMP, vec1) << std::endl;

    std::cout << "---------------MAXSIZE = " << MAXSIZE2 << "---------------" << std::endl;
    std::cout << "NoPar sum: " << noPar(vec2) << " time: " << timeCounter(noPar, vec2) << std::endl;
    std::cout << "SSE sum: " << noPar(vec2) << " time: " << timeCounter(SSE, vec2) << std::endl;
    std::cout << "Cascade sum: " << noPar(vec2) << " time: " << timeCounter(Cascade, vec2) << std::endl;
    std::cout << "forOpenMP sum: " << noPar(vec2) << " time: " << timeCounter(forOpenMP, vec2) << std::endl;
    std::cout << "Sections OpenMP sum: " << noPar(vec2) << " time: " << timeCounter(sectionsOpenMP, vec2) << std::endl;

    std::cout << "---------------MAXSIZE = " << MAXSIZE3 << "---------------" << std::endl;
    std::cout << "NoPar sum: " << noPar(vec3) << " time: " << timeCounter(noPar, vec3) << std::endl;
    std::cout << "SSE sum: " << noPar(vec3) << " time: " << timeCounter(SSE, vec3) << std::endl;
    std::cout << "Cascade sum: " << noPar(vec3) << " time: " << timeCounter(Cascade, vec3) << std::endl;
    std::cout << "forOpenMP sum: " << noPar(vec3) << " time: " << timeCounter(forOpenMP, vec3) << std::endl;
    std::cout << "Sections OpenMP sum: " << noPar(vec3) << " time: " << timeCounter(sectionsOpenMP, vec3) << std::endl;

    return 0;
}