// Anthony Genson  23/04/2020

#ifndef OCL_DEVICE_H
#define OCL_DEVICE_H

#ifdef OSX
#include <OpenCL/opencl.h>
#else
#include <CL/cl.h>
#endif

typedef struct {
    cl_device_type type;        // Device type (GPU / CPU)
    cl_platform_id platform;    // Platform ID
    cl_device_id device;        // Device ID
    cl_context context;         // OpenCL Context
    cl_command_queue cmdQueue;  // OpenCL Command-Queue
} Device;

/**
 * Obtain the list of platforms
 * 
 * @param p_platforms       A pointer to the list of platforms, will contain the list found
 *                              - set to NULL if none found
 * @param p_numPlatforms    A pointer to value, will contain the list length
 *                              - set to 0 if none found
 * 
 * @return Returns an error code (CL_SUCCESS if no error)
 */
cl_int getPlatforms(cl_platform_id **p_platforms, cl_uint *p_numPlatforms);

/**
 * Select a platform from a list that meets a device type requirement
 * 
 * @param numPlatforms  The number of platforms in the following list
 * @param platforms     A list of platforms
 * @param deviceType    The device type
 * @param p_platform    A pointer to value, which will contain the platform
 * 
 * @return              Returns an error code (CL_SUCCESS if no error)
 */
cl_int findPlatformFromType(cl_uint numPlatforms, cl_platform_id *platforms, cl_device_type deviceType, cl_platform_id *p_platform);

/**
 * Obtain the list of devices given a platform and device type
 * 
 * @param platform      The platform id
 * @param deviceType    The device type
 * @param p_devices     A pointer to the list of devices, will contain the list found
 *                          - set to NULL if none found
 * @param p_numDevices  A pointer to value, will contain the list length
 *                          - set to 0 if none found
 * 
 * @return              Returns an error code (CL_SUCCESS if no error)
 */
cl_int getDevices(cl_platform_id platform, cl_device_type deviceType, cl_device_id **p_devices, cl_uint *p_numDevices);

/**
 * Init OpenCL by:
 *      - Selecting a platform and device
 *      - Creating an OpenCL context and command-queue
 * 
 * @param deviceType    The default device type (GPU / CPU)
 * @param fallback      Boolean for allowing device type fallback or not
 * 
 * @return              Returns a Device struct containing all the necessary information
 */
Device initDevice(cl_device_type deviceType, cl_bool fallback);

/**
 * Release OpenCL context and command-queue
 */
void cleanDevice(Device device);

#endif /* OCL_DEVICE_H */
