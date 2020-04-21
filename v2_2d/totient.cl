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

__kernel void totient(const ulong lower, const ulong upper, __local ulong *localResults, __global ulong *groupResults) {
    uint globalID_0, globalID_1, groupID, localID, localSize, value = 0;
    ulong i, j, sum = 0;

    globalID_0 = get_global_id(0);
    globalID_1 = get_global_id(1);
    groupID = (get_group_id(0) * get_num_groups(1)) + get_group_id(1);
    localID = (get_local_id(0) * get_local_size(1)) + get_local_id(1);
    localSize = get_local_size(0) * get_local_size(1);

    // Increment local euler if relprime
    i = globalID_0 + lower;
    j = globalID_1 + 1;
    if (i <= upper && j < i) {
        // Increment local euler
        if (relprime(i, j)) { value++; }
    }
    localResults[localID] = value;

    // Sum all results in local scope (local sync)
    barrier(CLK_LOCAL_MEM_FENCE);
    if (localID == 0) {
        for (ulong i = 0; i < localSize; i++) {
            sum += localResults[i];
        }

        groupResults[groupID] = sum;
    }
}