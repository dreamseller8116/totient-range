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

__kernel void totient(const ulong lower, const ulong upper, __local ulong *localSum, __global ulong *result) {
    uint globalID, groupID, localID;
    ulong value;

    globalID = get_global_id(0);
    groupID = get_group_id(0);
    localID = get_local_id(0);
    
    if (globalID == 0) { *result = 0; }
    if (localID == 0) { *localSum = 0; }
    barrier(CLK_GLOBAL_MEM_FENCE);

    if (globalID + lower <= upper) {
        value = euler(globalID + lower);
        atom_add(localSum, value);
    }

    barrier(CLK_LOCAL_MEM_FENCE);
    if (localID == 0) { atom_add(result, *localSum); }
}
