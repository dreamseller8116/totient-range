# Parallel optimization with OpenCL

Using OpenCL to optimize the sequential code for totient sum.

- `totient.c`: sequential code
- `utils/`: timer + OpenCL configuration
- `*/v1/`: basic OpenCL implementation
- `*/v2/`: local sum reduction on kernel (local sync)
- `*/v3/`: modified v2 with atomic sum reduction
- `*/v4/`: extended v3 to add global sum reduction
- `*/v5/`: shorten v4 to just global atomic sum reduction
- `*/v6/`: modified v2 for parallel sum reduction (local sync)
- `*/v7/`: extended v6 to add global atomic sum reduction (like v4)
- `*/v8/`: modified v7 to divide to data size by 2 (work-item working twice as much)

All versions were first implemented in 1D sizes: `1d/*`

Then all versions were re-implemented for using 2D sizes: `2d/*`

## Usage

To initializa the project:
```
module load cuda10.0
make
```

To run one of the versions:
```
cd ./__DIMENSION__/__VERSION__/
srun --partition=amd-longq ./totient 1 15000 [localSize] [cpu]
```

- `lower upper`: lower and upper bounds for the totient sum
- `localSize`: (optional) 
    - `-1`:  will run a loop (default)
    - `0`:  will take its max possible value
    - Otherwise will take the value given
- `cpu`: (optional) whether or not you prefer using the cpu instead of gpu

### SBATCH

Test the versions:
- `sbatch test.sh ./__DIMENSION_/__VERSION__/ [localSize] [cpu]`
- `sbatch test-all.sh [localSize] [cpu]`

Benchmarking: (dimensions + versions + localSize)
- `sbatch benchmark-seq.sh` (sequential)
- `sbatch benchmark.sh`
