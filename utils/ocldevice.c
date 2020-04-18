#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "ocldevice.h"

#define ERROR(msg, ...) {                           \
    (void) fprintf(stderr, msg, ## __VA_ARGS__);    \
    (void) exit(1);                                 \
}

cl_int getPlatforms(cl_platform_id **p_platforms, cl_uint *p_numPlatforms) {
    cl_int status;

    *p_numPlatforms = 0;
    status = clGetPlatformIDs(0, NULL, p_numPlatforms);

    if (status == CL_SUCCESS && *p_numPlatforms > 0) {
        *p_platforms = (cl_platform_id*) malloc(*p_numPlatforms * sizeof(cl_platform_id));
        status = clGetPlatformIDs(*p_numPlatforms, *p_platforms, NULL);

        // Return immediately if a list of platforms is found, otherwise we clean before exiting the function
        if (status == CL_SUCCESS) { return status; }
        else { free(*p_platforms); }
    }

    *p_platforms = NULL;
    *p_numPlatforms = 0;

    return status;
}

cl_int findPlatformFromType(cl_uint numPlatforms, cl_platform_id *platforms, cl_device_type deviceType, cl_platform_id *p_platform) {
    cl_int status = CL_SUCCESS;
    cl_uint numDevices;

    for (cl_uint i = 0; i < numPlatforms; i++) {
        numDevices = 0;
        status = clGetDeviceIDs(platforms[i], deviceType, 0, NULL, &numDevices);

        // We stop the search as soon as we find a platform meeting the requirement
        if (status == CL_SUCCESS && numDevices > 0) {
            *p_platform = platforms[i];
            break;
        }
    }

    return status;
}

cl_int getDevices(cl_platform_id platform, cl_device_type deviceType, cl_device_id **p_devices, cl_uint *p_numDevices) {
    cl_int status;

    *p_numDevices = 0;
    status = clGetDeviceIDs(platform, deviceType, 0, NULL, p_numDevices);

    if (status == CL_SUCCESS && *p_numDevices > 0) {
        *p_devices = (cl_device_id*) malloc(*p_numDevices * sizeof(cl_device_id));
        status = clGetDeviceIDs(platform, deviceType, *p_numDevices, *p_devices, NULL);

        // Return immediately if a list of devices is found, otherwise we clean before exiting the function
        if (status == CL_SUCCESS) { return status; }
        else { free(*p_devices); }
    }

    *p_devices = NULL;
    *p_numDevices = 0;

    return status;
}

Device initDevice(cl_device_type deviceType, cl_bool fallback) {
    cl_device_type type;
    Device device;
    cl_int status;
    cl_uint numPlatforms = 0, numDevices = 0;
    cl_platform_id *platforms = NULL;
    cl_device_id *devices = NULL;

    // We obtain the list of platforms
    status = getPlatforms(&platforms, &numPlatforms);
    if (status != CL_SUCCESS || numPlatforms < 1) { ERROR("Error: Could not find any platform (code: %d)\n", status); }

    // We select a default device type (GPU / CPU)
    if (deviceType != CL_DEVICE_TYPE_GPU && deviceType != CL_DEVICE_TYPE_CPU) { type = CL_DEVICE_TYPE_GPU; }
    else { type = deviceType; }

    // We select a platform that meets the device type requirements (GPU and/or CPU)
    status = findPlatformFromType(numPlatforms, platforms, type, &(device.platform));
    if (status != CL_SUCCESS) {
        // We check if we are allowed to fallback to another device type
        if (fallback) {
            type = (type == CL_DEVICE_TYPE_GPU) ? CL_DEVICE_TYPE_CPU : CL_DEVICE_TYPE_GPU;
            status = findPlatformFromType(numPlatforms, platforms, type, &(device.platform));
        }

        if (status != CL_SUCCESS) { ERROR("Error: Could not find a platform meeting the device type requirements (code: %d)\n", status); }
    }
    device.type = type;

    // We obtain the list of devices on the selected platform, according to the device type selected
    status = getDevices(device.platform, device.type, &devices, &numDevices);
    if (status != CL_SUCCESS || numDevices < 1) { ERROR("Error: Could not retrieve any devices from the platform (code: %d)\n", status); }
    else { device.device = devices[0]; }

    // Init context
    device.context = clCreateContext(NULL, 1, &(device.device), NULL, NULL, &status);
    if (status != CL_SUCCESS || !(device.context)) { ERROR("Error: Could not create a context with the device (code: %d)\n", status); }

    // Init command-queue
    device.cmdQueue = clCreateCommandQueue(device.context, device.device, CL_QUEUE_PROFILING_ENABLE, &status);
    if (status != CL_SUCCESS || !(device.cmdQueue)) { ERROR("Error: Could not create a command queue for the device (code: %d)\n", status); }

    // We clean before exiting the function
    free(platforms);
    free(devices);

    return device;
}

void cleanDevice(Device device) {
    clReleaseCommandQueue(device.cmdQueue);
    clReleaseContext(device.context);
}
