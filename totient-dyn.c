// Greg Michaelson 14/10/2003
// Patrick Maier   29/01/2010 [enforced ANSI C compliance]
// Hugo Cousin     18/03/2020

#include <stdio.h>
#include <time.h>
#include <omp.h>

long hcf(long x, long y)
{
  long t;

  while (y != 0) {
    t = x % y;
    x = y;
    y = t;
  }
  return x;
}

int relprime(long x, long y)
{
  return hcf(x, y) == 1;
}

long euler(long n)
{
  long length;

  length = 0;
  for (long i = 1; i < n; i++)
    if (relprime(n, i))
      length += 1;
  return length;
}

long sumTotient(long lower, long upper)
{
  long sum = 0;

#pragma omp parallel for schedule(dynamic) num_threads(CORES) shared(lower, upper, sum) default(none)
  for (long i = lower; i <= upper; i++) {
    long x = euler(i);
#pragma omp atomic update
    sum += x;
  }
  return sum;
}

void runBenchmark(long lower, long upper)
{
  double start, end;
  double time_taken;

  start = omp_get_wtime();
  sumTotient(lower, upper);
  end = omp_get_wtime();
  time_taken = end - start;
  printf("%f\n", time_taken);
}

int main(int argc, char ** argv)
{
  long lower, upper;

  if (argc != 3) {
    printf("not 2 arguments\n");
    return 1;
  }
  sscanf(argv[1], "%ld", &lower);
  sscanf(argv[2], "%ld", &upper);
  runBenchmark(lower, upper);
  return 0;
}

