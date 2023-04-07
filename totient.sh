#!/bin/bash

#SBATCH --partition=amd-shortq
##-SBATCH -t00:15:00
##-SBATCH -J totient
#SBATCH -c 64

# Clear all loaded modules
module prune > /dev/null 2>&1

# Load necessary modules for the job
module load gcc slurm

run() {
    # Compile
    gcc -fopenmp -O3 $SLURM_JOB_NAME.c -o $SLURM_JOB_NAME-$SLURM_JOBID.out -DCORES="$1" -DCHUNKSIZE="$2"
    
    # Run the job
    srun -c64 $SLURM_JOB_NAME-$SLURM_JOBID.out $3 $4
    
    rm -f $SLURM_JOB_NAME-$SLURM_JOBID.out
}

bench_cores() {
    if [[ $# -ne 3 ]]; then
	echo "$0 RUNS LOWER_BOUND UPPER_BOUND"
	exit 1
    fi
    
    RUN="$1"
    LOWER="$2"
    UPPER="$3"
    
    for ((cores=1;cores<65;cores++))
    do
	echo -n "$cores,"
	for ((a=1;a<=$RUN;a++))
	do
	    run $cores 1 $LOWER $UPPER
	done | paste -s -d ','
    done
}


bench_chunksize() {
    if [[ $# -lt 3 ]]; then
	echo "$0 RUN LOWER_BOUND UPPER_BOUND"
	exit 1
    fi
    
    RUN="$1"
    LOWER="$2"
    UPPER="$3"

    for (( c=1; c<=8192; c*=2 ))
    do
	echo -n "$c,"
	for (( attempt=1; attempt<="$RUN"; attempt++ ))
	do
	    run 64 $c $LOWER $UPPER
	done | paste -s -d ','
    done
}

bench_cores "$@"
