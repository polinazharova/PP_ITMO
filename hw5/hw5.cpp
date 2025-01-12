#include <omp.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <atomic>

int N = 100000;

int min(std::vector<int> vec) {
    int min = vec[0];
#pragma omp parallel for 
    for (int i{ 1 }; i < vec.size(); i++) {
        if (vec[i] < min) {
#pragma omp critical
            if (vec[i] < min) {
                min = vec[i];
            }

        }
    }
    return min;
}
int max(std::vector<int> vec) {
    int max = vec[0];
#pragma omp parallel for 
    for (int i{ 1 }; i < vec.size(); i++) {
        if (vec[i] > max) {
#pragma omp critical
            if (vec[i] > max) {
                max = vec[i];
            }

        }
    }
    return max;
}

double timeCounter(int (*func)(const std::vector<int>& vec), const std::vector<int>& vec) {
    double start = omp_get_wtime();
    func(vec);
    double finish = omp_get_wtime();

    return finish - start;
}

int sumRed(const std::vector<int>& vec) {
    int sum = 0;
#pragma omp parallel for reduction(+:sum)
    for (int i{ 0 }; i < vec.size(); i++) {
        sum += vec[i];
    }
    std::cout << "Reduction sum: " << sum << std::endl;

    return sum;
}
int sumAtom(const std::vector<int>& vec) {
    int sum = 0;

#pragma omp parallel for
    for (int i{ 0 }; i < vec.size(); i++) {
#pragma omp atomic
        sum += vec[i];
    }
    std::cout << "Reduction sum with atomic: " << sum << std::endl;

    return sum;
}
int sumCrit(const std::vector<int>& vec) {
    int sum = 0;

#pragma omp parallel for
    for (int i{ 0 }; i < vec.size(); i++) {
#pragma omp critical
        sum += vec[i];
    }
    std::cout << "Reduction sum with critical: " << sum << std::endl;

    return sum;
}
int sumLocks(const std::vector<int>& vec) {
    omp_lock_t lock;
    omp_init_lock(&lock);
    int sum = 0;

#pragma omp parallel for
    for (int i{ 0 }; i < vec.size(); i++) {
        omp_set_lock(&lock);
        sum += vec[i];
        omp_unset_lock(&lock);
    }
    omp_destroy_lock(&lock);

    std::cout << "Reduction sum with locks: " << sum << std::endl;

    return sum;
}

int dotProduct(const std::vector<int>& vec1, const std::vector<int>& vec2) {
    if (vec1.size() != vec2.size()) {
        return -1;
    }

    int dot = 0;

    for (int i{ 0 }; i < vec1.size(); i++) {
        dot += vec1[i] * vec2[i];
    }

    return dot;
}
void vectorReading(const std::string& filename, std::vector<std::vector<int>>& vectors) {
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::vector<int> vec;
        int value;

        while (iss >> value) {
            vec.push_back(value);
        }

        vectors.push_back(vec);
    }
}
void task3(std::vector<std::vector<int>>& vectors) {
    std::atomic<bool> finished = false;
    omp_set_nested(1);
#pragma omp parallel sections 
    {
        #pragma omp section
            {
                vectorReading("vectors.txt", vectors);
                finished = true;
            }
        #pragma omp section
            {
                while(!finished) {}
                #pragma omp parallel for ordered
                for (int i{ 0 }; i < vectors.size(); i += 2) {
                    if (i + 1 < vectors.size()) {
                        int product = dotProduct(vectors[i], vectors[i + 1]);

                        #pragma omp ordered
                        std::cout << "Dot product of vectors " << i + 1 << " and " << i + 2 << ": " << product << std::endl;
                    }
                }
            }
    }
}

int main() {
    setlocale(LC_ALL, "rus");

    std::vector<int> vec;

    for (int i{ 1 }; i <= N; i++) {
        vec.push_back(i);
    }

    std::cout << "---------------TASK1--------------------" << std::endl;
    std::cout << "MIN: " << min(vec) << std::endl;
    std::cout << "MAX: " << max(vec) << std::endl;

    std::cout << "---------------TASK2--------------------" << std::endl;
    std::cout << "Reduction time: " << timeCounter(sumRed, vec) << std::endl;
    std::cout << "Reduction time with atomic: " << timeCounter(sumAtom, vec) << std::endl;
    std::cout << "Reduction time with critical: " << timeCounter(sumCrit, vec) << std::endl;
    std::cout << "Reduction time with locks: " << timeCounter(sumLocks, vec) << std::endl;

    std::cout << "---------------TASK3--------------------" << std::endl;
    std::vector<std::vector<int>> vectors;
    task3(vectors);

    return 0;
}