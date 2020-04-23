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

ulong euler(ulong n) {
    ulong length = 0;

    for (ulong i = 1; i < n; i++) {
        if (relprime(n, i)) { length++; }
    }

    return length;
}

__kernel void totient(const ulong lower, const ulong upper, __local ulong *localResult, __global ulong *result) {
    uint globalID, localID;
    ulong value;

    globalID = get_global_id(0);
    localID = get_local_id(0);
    
    // Init the result
    if (globalID == 0) { *result = 0; }
    barrier(CLK_GLOBAL_MEM_FENCE);
    // Init the local result
    if (localID == 0) { *localResult = 0; }
    barrier(CLK_LOCAL_MEM_FENCE);

    // Add private euler directly to local result
    if (globalID + lower <= upper) {
        value = euler(globalID + lower);
        atom_add(localResult, value);
    }

    // Add local result to global
    barrier(CLK_LOCAL_MEM_FENCE);
    if (localID == 0) { atom_add(result, *localResult); }
}
