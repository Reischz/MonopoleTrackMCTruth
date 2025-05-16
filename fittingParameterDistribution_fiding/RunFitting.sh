#!/bin/bash
#SBATCH --qos=cu_hpc
#SBATCH --partition=cpu
#SBATCH --tasks-per-node=8
#SBATCH --mem-per-cpu=8G
rm -rf FitParaDtata.txt
rm -rf Fitting_image/
mkdir Fitting_image
rm -rf *.root
for i in GEN*.log
do
    # wanting structure : linebigin end firstz lastz
    # saving as i_config
    # echo $(more sample.tt | awk '{print $1}' | grep -o '[0-9]\+')
    root "FindFittingPar.cpp(\"$i\")"
    #exit
done
