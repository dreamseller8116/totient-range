#!/bin/bash

#SBATCH --partition=amd-shortq
#SBATCH -J totient-seq
#SBATCH --error=sbatch-seq.err
#SBATCH --output=sbatch-seq.out

declare -a array=(15000 30000 100000)

for try in {1..3}; do
    output="benchmark-seq-$try.csv"
    [[ -f "$output" ]] && rm $output

    echo "Version,Lower,Upper,TotalTime,Result" >> $output

    for ds in ${array[@]}; do
        srun totient 1 $ds >> $output
    done
done
