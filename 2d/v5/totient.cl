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

__kernel void totient(const ulong lower, const ulong upper, __global ulong *result) {
    uint globalID_0, globalID_1;
    ulong i, j;

    globalID_0 = get_global_id(0);
    globalID_1 = get_global_id(1);

    // Init the result
    if (globalID_0 == 0 && globalID_1 == 0) { *result = 0; }
    barrier(CLK_GLOBAL_MEM_FENCE);

    // Calculate private relprime
    i = globalID_0 + lower;
    j = globalID_1 + 1;
    if (i <= upper && j < i) {
        // Increment directly the result
        if (relprime(i, j)) { atom_inc(result); }
    }
}
