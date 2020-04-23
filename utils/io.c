// Anthony Genson  23/04/2020

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"

void initArgs(int argc, char *argv[], ulong *p_lower, ulong *p_upper, int *p_localSize, char *deviceType) {
    *p_localSize = -1;

    // Check the program arguments
    if (argc < 3) { ERROR("Error: Need at least 3 arguments (lower upper [localSize] [cpu])\n"); }
    else if (argc > 3) { sscanf(argv[3], "%d", p_localSize); }

    sscanf(argv[1], "%ld", p_lower);
    sscanf(argv[2], "%ld", p_upper);
    strcpy(deviceType, (argc == 5) ? "CPU" : "GPU");
}

void save(char* version, ulong lower, ulong upper, char *deviceType, KernelRange range, double initTime, KernelTimer kernelTime, double totalTime, ulong result) {
    char filename[32];
    FILE *file;

    strcpy(filename, version);
    strcat(filename, ".csv");
    file = fopen(filename, "at");

    // If file is empty, add CSV header
    if (ftell(file) == 0) {
        fprintf(file, "Verison,Lower,Upper,Device,Dimensions,Global_0,Global_1,Global_2,Local_0,Local_1,Local_2,InitTime,KernelTime,DeviceTime,TotalTime,Result\n");
    }

    // Add the benchmark results
    fprintf(file, "%s,%ld,%ld,%s,%d,%ld,%ld,%ld,%ld,%ld,%ld,%.6f,%.6f,%.6f,%.6f,%ld\n", version, lower, upper, deviceType, (int)range.dim, (long)range.global[0], (long)range.global[1], (long)range.global[2], (long)range.local[0], (long)range.local[1], (long)range.local[2], initTime, kernelTime.kernel, kernelTime.gpu, totalTime, result);

    fclose(file);
}
