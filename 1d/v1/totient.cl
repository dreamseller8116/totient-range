// Anthony Genson  23/04/2020

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

__kernel void totient(const ulong lower, const ulong upper, __global ulong *results) {
    uint globalID;

    globalID = get_global_id(0);

    // Calculate private euler
    if (globalID + lower <= upper) {
        results[globalID] = euler(globalID + lower);
    }
}
