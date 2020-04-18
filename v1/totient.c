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
#define NUM_ARGS 3

int main(int argc, char ** argv) {
    ulong *result;
    char deviceType[4];
    size_t global, local;
    ulong lower, upper, count, sum = 0;
    Time start, initStop, stop;
    KernelArg args[NUM_ARGS];
    Device device;
    Kernel kernel;

    // Inputs / Output initialization
    if (argc != 3) { ERROR("Error: Need 2 arguments (lower and upper)\n"); }

    sscanf(argv[1], "%ld", &lower);
    sscanf(argv[2], "%ld", &upper);

    count = upper - lower + 1;
    result = (ulong*) malloc(count * sizeof(ulong));

    start = wcTime();
    
    // Init the device
    device = initDevice(CL_DEVICE_TYPE_GPU, CL_FALSE);
    strcpy(deviceType, (device.type == CL_DEVICE_TYPE_GPU) ? "GPU" : "CPU");

    // Create all the kernel arguments
    args[0] = createKernelArg(device, 0, Input, sizeof(ulong), 1, &lower);
    args[1] = createKernelArg(device, 1, Input, sizeof(ulong), 1, &upper);
    args[2] = createKernelArg(device, 2, Output, sizeof(ulong), count, result);

    // Init the kernel
    kernel = initKernel(device, KERNEL_NAME, SOURCE_FILE, NUM_ARGS, args);

    initStop = wcTime();

    // Run the kernel
    getMaxLocalSize(device, kernel, &local);
    if (count < local) local = count;

    global = ceil(count / (float)local) * local;

    runKernel(&kernel, device, 1, &global, &local);

    // Sum the results
    for (ulong i = 0; i < count; i++) { sum += result[i]; }

    stop = wcTime();

    // Benchmark
    printf("%s,%ld,%ld,%.6f,%.6f,%.6f,%.6f,%ld\n", deviceType, (long)global, (long)local, elapsedTime(start, initStop), kernel.timer.kernel, kernel.timer.gpu, elapsedTime(start, stop), sum);

    // Clean OpenCL
    cleanDevice(device);
    cleanKernel(kernel);
}
