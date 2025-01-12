#include <iostream>
#include <atomic>
#include <tbb/tbb.h>
#include <vector>
#include <execution>


int n = 100;

int forEach_t1(std::vector<int> vec) {
    int sum = 0;
    std::for_each(vec.begin(), vec.end(), [&sum](const int& elem) {
        sum += elem * elem;
        });
    return sum;
}
std::atomic<int> forEachPar_t1(std::vector<int> vec) {
    std::atomic<int> sum = 0;
    tbb::parallel_for_each(vec.begin(), vec.end(), [&sum](const int& elem) {
        sum.fetch_add(elem * elem);
        });
    return sum.load();
}
std::atomic<int> forEachIt_t1(std::vector<int> vec) {
    std::atomic<int> sum = 0;
    tbb::parallel_for(tbb::counting_iterator<int>(1), tbb::counting_iterator<int>(n), [&sum](const int& elem) {
        sum.fetch_add(elem * elem);
        });
    return sum.load();
}

int transformRed_t2(std::vector<int> vec) {
    int sum = std::transform_reduce(pstl::execution::par, vec.begin(), vec.end(), 0, std::plus<int>(), [](const int& elem) {
        return elem * elem;
        });

    return sum;
}
int parRed_t2() {
    int sum = tbb::parallel_reduce(
        tbb::blocked_range<int>(1, n+1),
        0,
        [&](tbb::blocked_range<int> r, int init)
        {
            int localSum = init;
            for (int i = r.begin(); i != r.end(); ++i)
            {
                localSum += i * i;
            }

            return localSum;
        }, std::plus<int>());

    return sum;
}


int main()
{
    std::atomic<int> sum = 0;
    std::vector<int> vec;


    for (int i{ 1 }; i <= n; i++) {
        vec.push_back(i);
    }

    std::cout << forEach_t1(vec) << std::endl;
    std::cout << parRed_t2() << std::endl;

    return 0;
}


