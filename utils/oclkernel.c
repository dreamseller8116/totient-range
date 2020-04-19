#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ocldevice.h"
#include "oclkernel.h"
#include "timer.h"

#define ERROR(msg, ...) {                           \
    (void) fprintf(stderr, msg, ## __VA_ARGS__);    \
    (void) exit(1);                                 \
}

void loadSource(char* filename, char *source, size_t *p_size) {
    FILE *file;
    size_t size = 0;

    // We open the source file
    file = fopen(filename, "r");
    if (file == NULL) ERROR("Error: Could not find the source file: %s\n", filename);

    // We retrieve the size necessary for buffer
    fseek(file, 0, SEEK_END);
    size = ftell(file) + 1;

    // If `p_size` is given, we provide the buffer size
    if (p_size != NULL) { *p_size = size; }

    // If `source` is given, we fill the buffer with the file content
    if (source != NULL) {
        rewind(file);
        source[size-1] = '\0';
        fread(source, sizeof(char), size-1, file);
    }

    fclose(file);
}

KernelArg createKernelArg(Device device, uint pos, IO io, size_t sizeOfType, uint length, void *values) {
    cl_mem_flags flag;
    KernelArg arg;
    cl_int status;

    arg.pos = pos;
    arg.io = io;
    arg.host.values = values;
    arg.host.length = length;

    // Total size of `values` (eg. if `int values[length]`, then `sizeof(int) * length`)
    arg.host.size = sizeOfType * length;

    // If io is None, then the argument do not need a buffer for either read nor write from host
    // Else, it requires the creation of a buffer
    if (arg.io != None) {
        flag = (arg.io == Memory) ? CL_MEM_HOST_NO_ACCESS : (arg.io == Input) ? CL_MEM_WRITE_ONLY : (arg.io == Output) ? CL_MEM_READ_ONLY : CL_MEM_READ_WRITE;
        arg.device.value = clCreateBuffer(device.context, flag, arg.host.size, NULL, &status);
        if (status != CL_SUCCESS || !(arg.device.value)) { ERROR("Error: Could not create the buffer for the kernel arg n°%d (code: %d)\n", pos, status); }

        // If the argument is write-only or read-write, we enqueue its writing
        if (arg.io == Input || arg.io == InputOutput) {
            status = clEnqueueWriteBuffer(device.cmdQueue, arg.device.value, CL_TRUE, 0, arg.host.size, arg.host.values, 0, NULL, NULL);
            if (status != CL_SUCCESS) { ERROR("Error: Could not enqueue the writing of buffer from kernel arg n°%d (code: %d)\n", pos, status); }
        }

        // Will be used to know whether or not a memory object was created for this argument
        arg.device.size = sizeof(cl_mem);
    } else { arg.device.size = 0; }

    return arg;
}

Kernel initKernel(Device device, char *name, char *filename) {
    size_t sourceSize, logSize;
    char *source, *log;
    Kernel kernel;
    cl_int status;

    // Load the source code
    loadSource(filename, NULL, &sourceSize);
    source = (char*) malloc(sourceSize * sizeof(char));
    loadSource(filename, source, NULL);

    // Create the program
    kernel.program = clCreateProgramWithSource(device.context, 1, (const char **) &source, NULL, &status);
    if (status != CL_SUCCESS || !(kernel.program)) { ERROR("Error: Could not create the program (code: %d)\n", status); }
    free(source);

    // Build the program
    status = clBuildProgram(kernel.program, 0, NULL, NULL, NULL, NULL);
    if (status != CL_SUCCESS) {
        clGetProgramBuildInfo(kernel.program, device.device, CL_PROGRAM_BUILD_LOG, 0, NULL, &logSize);
        log = (char*) malloc(logSize * sizeof(char));
        clGetProgramBuildInfo(kernel.program, device.device, CL_PROGRAM_BUILD_LOG, logSize, log, NULL);
        ERROR("Error: Could not build the program (code: %d):\n%s\n", status, log);
    }

    // Create the kernel from the program
    kernel.kernel = clCreateKernel(kernel.program, name, &status);
    if (status != CL_SUCCESS || !(kernel.kernel)) { ERROR("Error: Could not create the kernel (code: %d)\n", status); }

    return kernel;
}

void getMaxLocalSize(Device device, Kernel kernel, size_t *p_localSize) {
    cl_int status;

    status = clGetKernelWorkGroupInfo(kernel.kernel, device.device, CL_KERNEL_WORK_GROUP_SIZE, sizeof(size_t), p_localSize, NULL);
    if (status != CL_SUCCESS) { ERROR("Error: Could not retrieve the maximum local size (code: %d)\n", status); }
}

void checkRangeSizes(Kernel kernel, Device device, KernelRange range) {
    size_t localSize;

    getMaxLocalSize(device, kernel, &localSize);

    for (uint i = 0; i < range.dim; i++) {
        if (range.local[i] > localSize) { ERROR("Error: The local size [%d] exceeds its maximum (eg. %zu)\n", i, localSize); }
        if (range.global[i] % range.local[i] != 0) { ERROR("Error: The global size needs to be divisable by the local size in [%d]\n", i); }
    }
}

void initKernelArgs(Kernel *p_kernel, uint numArgs, KernelArg *args) {
    uint argSize = 0;
    void *argValue;
    cl_int status;

    // Set all the kernel arguments
    p_kernel->args = args;
    p_kernel->numArgs = numArgs;
    for (uint i = 0; i < p_kernel->numArgs; i++) {
        // If the argument is an array, then we provide `kernelArg.device` information, otherwise `kernelArg.host`
        argValue = (p_kernel->args[i].device.size != 0) ? &(p_kernel->args[i].device.value) : p_kernel->args[i].host.values;
        argSize = (p_kernel->args[i].device.size != 0) ? p_kernel->args[i].device.size : p_kernel->args[i].host.size;

        status = clSetKernelArg(p_kernel->kernel, p_kernel->args[i].pos, argSize, argValue);
        if (status != CL_SUCCESS) { ERROR("Error: Could not set kernel arg n°%d (code: %d)\n", p_kernel->args[i].pos, status); }
    }
}

void initKernelRange(KernelRange *p_range, cl_uint dim, ulong dataSize, size_t localSize) {
    size_t globalSize;

    p_range->dim = (dim < 1 || dim > 3) ? 1 : dim;
    // Round up the global size to be divisable by the local size
    globalSize = ceil(dataSize / (float)localSize) * localSize;

    for (uint i = 0; i < dim; i++) {
        p_range->local[i] = localSize;
        p_range->global[i] = globalSize;
    }
}

void runKernel(Kernel *p_kernel, Device device, KernelRange range) {
    Time startKernel, stopKernel;
    cl_ulong startGPU, stopGPU;
    cl_event event;
    cl_int status;

    startKernel = wcTime();

    // First heck the arguments
    checkRangeSizes(*p_kernel, device, range);

    // Enqueue the kernel execution
    status = clEnqueueNDRangeKernel(device.cmdQueue, p_kernel->kernel, range.dim, NULL, range.global, range.local, 0, NULL, &event);
    if (status != CL_SUCCESS) { ERROR("Error: Could not run the kernel (code: %d)\n", status); }

    // Wait for the kernel to finish
    status = clFinish(device.cmdQueue);
    if (status != CL_SUCCESS) { ERROR("Error: Could not finish the command-queue from kernel execution (code: %d)\n", status); }

    stopKernel = wcTime();
    p_kernel->timer.kernel = elapsedTime(startKernel, stopKernel);

    // Retrieve profiling information
    status = clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &startGPU, NULL);
    status |= clGetEventProfilingInfo(event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &stopGPU, NULL);
    if (status != CL_SUCCESS) { ERROR("Error: Could not retrive profiling info (code: %d)\n", status); }
    else { p_kernel->timer.gpu = (double) (stopGPU - startGPU) / 1000000.0; }

    // For each kernel argument
    for (uint i = 0; i < p_kernel->numArgs; i++) {
        // Retrieve buffer if exists and is read-only or read-write
        if (p_kernel->args[i].device.size != 0 && p_kernel->args[i].io != Memory && p_kernel->args[i].io != Input) {
            status = clEnqueueReadBuffer(device.cmdQueue, p_kernel->args[i].device.value, CL_TRUE, 0, p_kernel->args[i].host.size, p_kernel->args[i].host.values, 0, NULL, NULL);
            if (status != CL_SUCCESS) { ERROR("Error: Could not enqueue the reading of buffer back to the host (arg n°%d) (code: %d)\n", p_kernel->args[i].pos, status); }
        }
    }
}

void cleanKernel(Kernel kernel) {
    for (uint i = 0; i < kernel.numArgs; i++) {
        // Release each memory objects that might have been created
        if (kernel.args[i].device.size != 0) clReleaseMemObject(kernel.args[i].device.value);
    }

    clReleaseKernel(kernel.kernel);
    clReleaseProgram(kernel.program);
}
