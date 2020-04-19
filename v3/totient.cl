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

__kernel void totient(const ulong lower, const ulong upper, __global ulong *groupResults) {
    uint globalID, groupID, localID;
    ulong value;

    globalID = get_global_id(0);
    groupID = get_group_id(0);
    localID = get_local_id(0);
    
    if (localID == 0) { groupResults[groupID] = 0; }
    barrier(CLK_LOCAL_MEM_FENCE);

    if (globalID + lower <= upper) {
        value = euler(globalID + lower);
        atom_add(&(groupResults[groupID]), value);
    }
}
