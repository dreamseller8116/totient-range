#include <stdio.h>
#include <string.h>

#include "../utils/ocldevice.h"
#include "../utils/oclkernel.h"
#include "../utils/timer.h"
#include "../utils/io.h"

#define VERSION "v7"
#define SOURCE_FILE "totient.cl"
#define KERNEL_NAME "totient"
#define NUM_ARGS 4

size_t benchmark(ulong lower, ulong upper, size_t localSize, char *deviceType) {
    size_t maxLocalSize;
    ulong dataSize, sum;
    Time start, initStop, stop;
    KernelArg args[NUM_ARGS];
    KernelRange range;
    Device device;
    Kernel kernel;

    start = wcTime();
    
    // Init the device
    device = initDevice((strcmp(deviceType, "CPU") == 0) ? CL_DEVICE_TYPE_CPU : CL_DEVICE_TYPE_GPU, CL_FALSE);

    // Init the kernel
    kernel = initKernel(device, KERNEL_NAME, SOURCE_FILE);

    // Init the kernel range sizes
    dataSize = upper - lower + 1;
    getMaxLocalSize(device, kernel, &maxLocalSize);
    if (localSize > maxLocalSize) { ERROR("Error: Local size exceeds its bounds\n"); }
    else if (localSize == 0) { initKernelRange1D(&range, dataSize, maxLocalSize); }
    else { initKernelRange1D(&range, dataSize, localSize); }

    // Create all the kernel arguments
    args[0] = createKernelArg(device, 0, None, sizeof(ulong), 1, &lower);
    args[1] = createKernelArg(device, 1, None, sizeof(ulong), 1, &upper);
    args[2] = createKernelArg(device, 2, None, sizeof(ulong), range.local[0], NULL);
    args[3] = createKernelArg(device, 3, Output, sizeof(ulong), 1, &sum);
    initKernelArgs(&kernel, NUM_ARGS, args);

    initStop = wcTime();

    // Run the kernel
    runKernel(&kernel, device, range);

    stop = wcTime();

    // Benchmark
    save(VERSION, lower, upper, deviceType, range, elapsedTime(start, initStop), kernel.timer, elapsedTime(start, stop), sum);

    // Clean OpenCL
    cleanDevice(device);
    cleanKernel(kernel);

    return maxLocalSize;
}

void runBenchmark(ulong lower, ulong upper, int localSize, char *deviceType) {
    size_t maxLocalSize = 1;

    // localSize if (localSize > 0), maxLocalSize if (localSize == 0)
    if (localSize >= 0) { benchmark(lower, upper, localSize, deviceType); }
    else {
        // Else we run the benchmark in loop
        // By running once and retrieving the maxLocalSize
        maxLocalSize = benchmark(lower, upper, 2, deviceType);
        for (uint i = 4; i <= maxLocalSize; i *= 4) {
            benchmark(lower, upper, i, deviceType);
        }
    }
}

int main(int argc, char **argv) {
    char deviceType[4];
    ulong lower, upper;
    int localSize;

    // Init variables
    initArgs(argc, argv, &lower, &upper, &localSize, deviceType);

    // Run the benchmark
    runBenchmark(lower, upper, localSize, deviceType);
}
