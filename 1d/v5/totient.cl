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

__kernel void totient(const ulong lower, const ulong upper, __global ulong *result) {
    uint globalID;
    ulong value;

    globalID = get_global_id(0);
    
    // Init the result
    if (globalID == 0) { *result = 0; }
    barrier(CLK_GLOBAL_MEM_FENCE);

    // Add private euler directly to result
    if (globalID + lower <= upper) {
        value = euler(globalID + lower);
        atom_add(result, value);
    }
}
