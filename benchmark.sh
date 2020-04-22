#!/bin/bash

#SBATCH --partition=amd-longq
#SBATCH --exclusive
#SBATCH -J totient
#SBATCH --error=sbatch.err
#SBATCH --output=sbatch.out

declare -a array=(15000 30000 100000)
declare -a folders=(1d 2d)

declare -a local1=(2 4 16 64 256 1024)
declare -a local2=(1 4 16 64 256 1024)

for try in {1..3}; do
    find . -type f -name '*d-v*.csv' -delete

    for folder in ${folders[@]}; do
        cd folder

        for ds in ${array[@]}; do
            for version in $(ls -d v*/); do    
                cd $version

                if [[ folder == "1d" ]]; then
                    for localSize in ${local1[@]}; do
                        srun totient 1 $ds $localSize
                        srun totient 1 $ds $localSize cpu
                    done
                else
                    for localSize in ${local2[@]}; do
                        srun totient 1 $ds $localSize
                        srun totient 1 $ds $localSize cpu
                    done
                fi

                cd ..
            done
        done

        cd ..
    done

    ./gather.sh "benchmark-$try.csv"
done
