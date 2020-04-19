#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "../utils/ocldevice.h"
#include "../utils/oclkernel.h"
#include "../utils/timer.h"

#define ERROR(msg, ...) {                           \
    (void) fprintf(stderr, msg, ## __VA_ARGS__);    \
    (void) exit(1);                                 \
}

#define SOURCE_FILE "totient.cl"
#define KERNEL_NAME "totient"
#define NUM_ARGS 4

int main(int argc, char ** argv) {
    char deviceType[4];
    ulong lower, upper, dataSize, numGroups, sum;
    Time start, initStop, stop;
    KernelArg args[NUM_ARGS];
    KernelRange range;
    Device device;
    Kernel kernel;

    // Inputs / Output initialization
    if (argc < 3) { ERROR("Error: Need at least 2 arguments (lower upper [cpu])\n"); }

    sscanf(argv[1], "%ld", &lower);
    sscanf(argv[2], "%ld", &upper);

    dataSize = upper - lower + 1;

    start = wcTime();
    
    // Init the device
    device = initDevice((argc == 4) ? CL_DEVICE_TYPE_CPU : CL_DEVICE_TYPE_GPU, CL_FALSE);
    strcpy(deviceType, (device.type == CL_DEVICE_TYPE_GPU) ? "GPU" : "CPU");

    // Init the kernel
    kernel = initKernel(device, KERNEL_NAME, SOURCE_FILE);

    range.dim = 1;
    getMaxLocalSize(device, kernel, range.local);
    if (dataSize < range.local[0]) range.local[0] = dataSize;
    numGroups = ceil(dataSize / (float)range.local[0]);
    range.global[0] = numGroups * range.local[0];

    // Create all the kernel arguments
    args[0] = createKernelArg(device, 0, None, sizeof(ulong), 1, &lower);
    args[1] = createKernelArg(device, 1, None, sizeof(ulong), 1, &upper);
    args[2] = createKernelArg(device, 2, None, sizeof(ulong), 1, NULL);
    args[3] = createKernelArg(device, 3, Output, sizeof(ulong), 1, &sum);
    initKernelArgs(&kernel, NUM_ARGS, args);

    initStop = wcTime();

    // Run the kernel
    runKernel(&kernel, device, range);

    stop = wcTime();

    // Benchmark
    printf("%s,%d,%ld,%ld,%.6f,%.6f,%.6f,%.6f,%ld\n", deviceType, (int)range.dim, (long)range.global[0], (long)range.local[0], elapsedTime(start, initStop), kernel.timer.kernel, kernel.timer.gpu, elapsedTime(start, stop), sum);

    // Clean OpenCL
    cleanDevice(device);
    cleanKernel(kernel);
}
