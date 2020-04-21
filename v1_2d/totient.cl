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

__kernel void totient(const ulong lower, const ulong upper, __global uint *results) {
    uint globalID_0, globalID_1, globalID, value = 0;
    ulong i, j, sum = 0;

    globalID_0 = get_global_id(0);
    globalID_1 = get_global_id(1);
    globalID = (globalID_0 * get_global_size(1)) + globalID_1;

    // Increment local euler if relprime
    i = globalID_0 + lower;
    j = globalID_1 + 1;
    if (i <= upper && j < i) {
        // Increment local euler
        if (relprime(i, j)) { value++; }
    }
    results[globalID] = value;
}
