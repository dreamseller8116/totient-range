# Parallel optimization with OpenCL

Using OpenCL to optimize the sequential code for totient sum.

- `totient.c`: sequential code
- `utils/`: timer + OpenCL configuration
- `v1/`: basic OpenCL implementation
- `v2/`: local sum reduction on kernel

## Usage

To initializa the project:
```
module load cuda10.0
make
```

To run one of the versions:
```
cd ./__VERSION__/
srun --partition=amd-longq ./totient 1 15000 [cpu]
```
The last argument is optional, whether or not you prefer using the cpu instead of gpu.