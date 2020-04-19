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

ulong euler(ulong n) {
    ulong length = 0;

    for (ulong i = 1; i < n; i++) {
        if (relprime(n, i)) { length++; }
    }

    return length;
}

__kernel void totient(const ulong lower, const ulong upper, __local ulong *localSums, __global ulong *result) {
    uint globalID, localID, localSize;
    ulong value = 0;

    globalID = get_global_id(0);
    localID = get_local_id(0);
    localSize = get_local_size(0);

    if (globalID == 0) { *result = 0; }
    barrier(CLK_GLOBAL_MEM_FENCE);

    if (globalID + lower <= upper) { value = euler(globalID + lower); }

    localSums[localID] = value;
    for (uint offset = localSize/2; offset > 0; offset /= 2) {
        barrier(CLK_LOCAL_MEM_FENCE);
        if (localID < offset) { localSums[localID] += localSums[localID + offset]; }
    }

    barrier(CLK_LOCAL_MEM_FENCE);
    if (localID == 0) { atom_add(result, localSums[0]); }
}
