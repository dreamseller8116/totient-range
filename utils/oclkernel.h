#ifndef OCL_KERNEL_H
#define OCL_KERNEL_H

#ifdef OSX
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

typedef struct {
    double gpu;         // Time spent on GPU
    double kernel;      // Time spent on Kernel
} KernelTimer;

typedef enum {
    Input,              // CL_MEM_WRITE_ONLY as tag for the creation of buffer
    Output,             // CL_MEM_READ_ONLY as tag for the creation of buffer
    InputOutput         // CL_MEM_READ_WRITE as tag for the creation of buffer
} IO;

typedef struct {
    size_t size;        // Total size of `values` (eg. sizeof(TYPE) * length)
    uint length;        // Length of `values` (eg. number of elements in array)
    void *values;       // Array of values (can be any type)
} HostValue;

typedef struct {
    size_t size;        // Equal to sizeof(cl_mem) if a value is defined, otherwise 0
    cl_mem value;       // Memory object created from `clCreateBuffer`
} DeviceValue;

typedef struct {
    uint pos;           // Position in the list of kernel arguments
    IO io;              // Whether the argument is read-only, write-only or read-write
    HostValue host;     // Argument value on the host
    DeviceValue device; // Argument value on the device, set only if the host value is an array
} KernelArg;

typedef struct {
    KernelTimer timer;  // Timer information once the kernel was executed
    cl_program program; // OpenCL program
    cl_kernel kernel;   // OpenCL kernel
    KernelArg *args;    // Arguments for the kernel
    uint numArgs;       // Number of kernel arguments
} Kernel;

/**
 * Load the source code from a given filename
 * 
 * @note Either argument `source` of `p_size` can be NULL
 * @note Set `source` to NULL to retreive the size necessary for its allocation
 * 
 * @param filename  The filename of the source code
 * @param source    A string, which will contain the source code (terminats with '\0')
 *                      - needs to have the necessary allocated memory
 * @param p_size    A pointer to value, representing the size of the source string
 */
void loadSource(char* filename, char *source, size_t *p_size);

/**
 * Create an argument for the kernel
 * 
 * @note Creates a memory object if it is an array
 * @note Enqueue the writing of the memory object if write-only or read-write
 * 
 * @param device        A Device struct, containing OpenCL context and cmdQueue
 * @param pos           The position in the list of kernel arguments
 * @param io            An IO enum value, for whether read-only, write-only or read-write
 * @param sizeOfType    The size of the array type (eg. if `int`, then `sizeof(int)`)
 * @param length        The length of `values` (eg. number of elements in array)
 * @param values        Array of values (can be any type)
 * 
 * @return              Returns a KernelArg struct with all the necessary information
 */
KernelArg createKernelArg(Device device, uint pos, IO io, size_t sizeOfType, uint length, void *values);

/**
 * Init a kernel by:
 *      - Loading, creating and building the program
 *      - Creating the kernel
 *      - Setting all the arguments for the kernel
 * 
 * @param device    A Device struct, containing device ID and OpenCL context
 * @param name      The name of the kernel
 * @param filename  The filename of the source code
 * @param numArgs   The number of kernel arguments
 * @param args      The list of kernel arguments, each formatted as KernelArg struct
 * 
 * @return          Returns a Kernel struct with all the necessary information
 */
Kernel initKernel(Device device, char *name, char *filename, uint numArgs, KernelArg *args);

/**
 * Get the maximum local size that can be used for a given kernel
 * 
 * @param kernel        A Kernel struct
 * @param device        A Device struct
 * @param p_localSize   A pointer to value, will contain the maximum local size
 */
void getMaxLocalSize(Device device, Kernel kernel, size_t *p_localSize);

/**
 * Check the global and local size for a kernel
 * 
 * @param kernel    A Kernel struct
 * @param device    A Device struct
 * @param dim       The number of dimensions for `global` and `local`
 * @param global    The number of global work-items
 * @param local     The number of work-items per work-group
 */
void checkRangeSizes(Kernel kernel, Device device, cl_uint dim, size_t *global, size_t *local);

/**
 * Run a given kernel and retreive the execution results
 * 
 * @param p_kernel  A pointer to a Kernel struct
 * @param device    A Device struct, containing device ID and OpenCL context
 * @param dim       The number of dimensions for `global` and `local`
 * @param global    The number of global work-items
 * @param local     The number of work-items per work-group
 */
void runKernel(Kernel *p_kernel, Device device, cl_uint dim, size_t *global, size_t *local);

/**
 * Release:
 *      - OpenCL kernel and program
 *      - Memory object that might have been created for kernel arguments
 */
void cleanKernel(Kernel kernel);

#endif /* OCL_KERNEL_H */
