#!/bin/bash

#SBATCH --partition=amd-longq
#SBATCH -t00:05:00
#SBATCH -J totient
#SBATCH --error=sbatch.err
#SBATCH --output=sbatch.out

localSize=-1

if [[ "$#" -gt 0 ]]; then
    localSize=$1
fi

for version in $(ls -d v*/); do    
    cd $version

    find . -type f -name '*.csv' -delete

    if [[ "$#" -gt 1 ]]; then
        srun totient 1 10000 $localSize cpu
    else
        srun totient 1 10000 $localSize
    fi

    cd ..
done
