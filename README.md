# Parallel optimization with OpenCL

Using OpenCL to optimize the sequential code for totient sum.

- `totient.c`: sequential code
- `utils/`: timer + OpenCL configuration
- `v1/`: basic OpenCL implementation
- `v2/`: local sum reduction on kernel (local sync)
- `v3/`: modified v2 with atomic sum reduction
- `v4/`: extended v3 to add global sum reduction
- `v5/`: shorten v4 to just global atomic sum reduction
- `v6/`: modified v2 for parallel sum reduction (local sync)
- `v7/`: extended v6 to add global atomic sum reduction (like v4)

All versions ending with `*_2d` are the versions re-implemented for 2D.

## Usage

To initializa the project:
```
module load cuda10.0
make
```

To run one of the versions:
```
cd ./__VERSION__/
srun --partition=amd-longq ./totient 1 15000 [localSize] [cpu]
```

- `lower upper`: lower and upper bounds for the totient sum
- `localSize`: (optional) 
    - `-1`:  will run a loop (default)
    - `0`:  will take its max possible value
    - Otherwise will take the value given
- `cpu`: (optional) whether or not you prefer using the cpu instead of gpu
