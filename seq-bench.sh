#!/bin/bash

#SBATCH --partition=amd-shortq
#SBATCH -t00:15:00
#SBATCH -J totient-seq
#SBATCH --exclusive

# Clear all loaded modules
module prune > /dev/null 2>&1

# Load necessary modules for the job
module load gcc slurm

run() {
    # Compile
    gcc -O3 TotientRange.c -o $SLURM_JOB_NAME-$SLURM_JOBID.out 

    # Run the job
    srun $SLURM_JOB_NAME-$SLURM_JOBID.out $1 $2
    
    rm -f $SLURM_JOB_NAME-$SLURM_JOBID.out
}

bench() {
    if [[ $# -ne 3 ]]; then
	echo "$0 RUNS LOWER_BOUND UPPER_BOUND"
	exit 1
    fi
    
    RUN="$1"
    LOWER="$2"
    UPPER="$3"
       
    for ((a=1;a<=$RUN;a++))
    do
	run $LOWER $UPPER
    done | paste -s -d ','
}

bench "$@"
