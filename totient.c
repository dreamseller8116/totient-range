#include <stdio.h>

#include "utils/timer.h"

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

    for (int i = 1; i < n; i++)
        if (relprime(n, i))
        length++;

    return length;
}

long sumTotient(long lower, long upper) {
    long sum = 0;

    for (int i = lower; i <= upper; i++)
        sum = sum + euler(i);

    return sum;
}

void runBenchmark(long lower, long upper) {
    Time start, stop;
    double time;
    long sum;

    start = wcTime();
    sum = sumTotient(lower, upper);
    stop = wcTime();

    time = elapsedTime(start, stop);
    printf("seq,%ld,%ld,%.6f,%ld", lower, upper, time, sum);
}

int main(int argc, char ** argv) {
    long lower, upper;

    if (argc != 3) {
        fprintf(stderr, "Error: need 2 arguments.\n");
        return 1;
    }

    sscanf(argv[1], "%ld", &lower);
    sscanf(argv[2], "%ld", &upper);
    runBenchmark(lower, upper);

    return 0;
}
