#include <iostream>
#include <cstdlib>
#include <chrono>
#include <omp.h>

long hcf(long x, long y) {
    long t;
    while (y != 0) {
        t = x % y;
        x = y;
        y = t;
    }
    return x;
}

int relprime(long x, long y) {
    return hcf(x, y) == 1;
}

long euler(long n) {
    long length = 0;
#pragma omp parallel for reduction(+:length)
    for (long i = 1; i < n; i++) {
        if (relprime(n, i)) {
            length++;
        }
    }
    return length;
}

long sumTotient(long lower, long upper) {
    long sum = 0;
#pragma omp parallel for reduction(+:sum)
    for (long i = lower; i <= upper; i++) {
        sum += euler(i);
    }
    return sum;
}

void runBenchmark(long lower, long upper, int num_threads) {
    std::cout << "Running benchmark for Euler totient function with " << num_threads << " threads\n";
    double start, end, time_taken;
    long result;

    // Set number of threads
    omp_set_num_threads(num_threads);

    // Parallel version
    start = omp_get_wtime();
    result = sumTotient(lower, upper);
    end = omp_get_wtime();
    time_taken = end - start;

    std::cout << "Result: " << result << ", execution time: " << time_taken << " seconds\n";
}

int main() {
    long lower, upper;
    int num_threads;

    while (true) {
        std::cout << "Enter the lower bound (0 to exit): ";
        std::cin >> lower;
        if (lower == 0) {
            break;
        }

        std::cout << "Enter the upper bound: ";
        std::cin >> upper;

        std::cout << "Enter the number of threads: ";
        std::cin >> num_threads;

        runBenchmark(lower, upper, num_threads);
    }

    return 0;
}
