// Anthony Genson  23/04/2020

#pragma OPENCL EXTENSION cl_khr_int64_base_atomics: enable

ulong hcf(ulong x, ulong y) {
    ulong t;

    while (y != 0) {
        t = x % y;
        x = y;
        y = t;
    }

    return x;
}

int relprime(ulong x, ulong y) {
    return hcf(x, y) == 1;
}

__kernel void totient(const ulong lower, const ulong upper, __local ulong *localResults, __global ulong *result) {
    uint globalID_0, globalID_1, localID, localSize, value = 0;
    ulong i, j;

    globalID_0 = get_global_id(0);
    globalID_1 = get_global_id(1);
    localID = (get_local_id(0) * get_local_size(1)) + get_local_id(1);
    localSize = get_local_size(0) * get_local_size(1);

    // Init the result
    if (globalID_0 == 0 && globalID_1 == 0) { *result = 0; }
    barrier(CLK_GLOBAL_MEM_FENCE);

    // Increment local euler if relprime
    // Run twice
    j = globalID_1 + 1;
    for (uint iter = 0; iter < 2; iter++) {
        i = (2 * globalID_0) + lower + iter;
        if (i <= upper && j < i) {
            // Increment local euler
            if (relprime(i, j)) { value++; }
        }
    }
    
    // Parallel sum reduction in local scope
    // https://www.cl.cam.ac.uk/teaching/1617/AdvGraph/07_OpenCL.pdf
    localResults[localID] = value;
    for (uint offset = localSize/2; offset > 0; offset /= 2) {
        barrier(CLK_LOCAL_MEM_FENCE);
        if (localID < offset) { localResults[localID] += localResults[localID + offset]; }
    }

    // Add local result to global
    barrier(CLK_LOCAL_MEM_FENCE);
    if (localID == 0) { atom_add(result, localResults[0]); }
}
