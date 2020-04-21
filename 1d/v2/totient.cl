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

__kernel void totient(const ulong lower, const ulong upper, __local ulong *localResults, __global ulong *groupResults) {
    uint globalID, groupID, localID, localSize;
    ulong sum = 0;

    globalID = get_global_id(0);
    groupID = get_group_id(0);
    localID = get_local_id(0);
    localSize = get_local_size(0);

    // Calculate private euler
    if (globalID + lower <= upper) { localResults[localID] = euler(globalID + lower); }
    else { localResults[localID] = 0; }

    // Sum all results in local scope (local sync)
    barrier(CLK_LOCAL_MEM_FENCE);
    if (localID == 0) {
        for (ulong i = 0; i < localSize; i++) {
            sum += localResults[i];
        }

        groupResults[groupID] = sum;
    }
}
