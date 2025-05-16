#!/bin/bash
#SBATCH --qos=cu_hpc
#SBATCH --partition=cpu
#SBATCH --tasks-per-node=8
#SBATCH --mem-per-cpu=8G

rm -rf testfile

for (( i=15;i<7001;i+=15))
do
    echo "launch PROC_mono_spinhalf_0 -n mass_${i}GeV" >> "testfile"
    echo "set mass 4110000 ${i}" >> "testfile"
    echo "" >> "testfile"
done