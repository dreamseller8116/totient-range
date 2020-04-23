// Anthony Genson  23/04/2020

#ifndef IO_H
#define IO_H

#include "oclkernel.h"

#define ERROR(msg, ...) {                           \
    (void) fprintf(stderr, msg, ## __VA_ARGS__);    \
    (void) exit(1);                                 \
}

/**
 * Init the program variables from given program arguments
 * 
 * @param argc          argc from main
 * @param argv          argv from main
 * @param p_lower       Pointer to value, will contain lower bound
 * @param p_upper       Pointer to value, will contain upper bound
 * @param p_localSize   Pointer to value, will contain local size
 * @param deviceType    Will be equal to either `GPU` or `CPU`
 */
void initArgs(int argc, char *argv[], ulong *p_lower, ulong *p_upper, int *p_localSize, char *deviceType);

/**
 * Save the benchmark results to a CSV file (create if not exists)
 * 
 * @param version       The benchmark version (used for filename)
 * @param lower         The lower bound for totient sum
 * @param upper         The upper bound for totient sum
 * @param deviceType    Equal to either `GPU` or `CPU`
 * @param range         The KernelRange struct used for dimensions and sizes
 * @param initTime      The time elapsed for init
 * @param kernelTime    The time elapsed on kernel
 * @param totalTime     The time elapsed on device
 * @param result        The result of totient sum
 */
void save(char* version, ulong lower, ulong upper, char *deviceType, KernelRange range, double initTime, KernelTimer kernelTime, double totalTime, ulong result);

#endif /* IO_H */
