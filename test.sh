#!/bin/bash

#SBATCH --partition=amd-longq
#SBATCH -t00:05:00
#SBATCH -J totient
#SBATCH --error=sbatch.err
#SBATCH --output=sbatch.out

localSize=-1

if [[ "$#" -lt 1 ]]; then
    echo "Error: needs at least 1 argument (folder [localSize] [cpu])." 1>&2
    exit 128
fi

if [[ "$#" -gt 1 ]]; then
    localSize=$2
fi

cd $1

[[ -f "$1.csv" ]] && rm "$1.csv"

if [[ "$#" -gt 2 ]]; then
    srun totient 1 10000 $localSize cpu
else
    srun totient 1 10000 $localSize
fi