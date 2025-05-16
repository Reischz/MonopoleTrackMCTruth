#!/bin/bash
#SBATCH --qos=cu_hpc
#SBATCH --partition=cpu
#SBATCH --tasks-per-node=8
#SBATCH --mem-per-cpu=8G

# colloect xsection
dir=$(pwd)
looking="PROC_mono_spinhalf_0"
rm -rf "DYL_nn23lo1_mass_xsec-pb.log"
for (( i=15;i<7001;i+=15))
do
    echo -n "${i} " >> "DYL_nn23lo1_mass_xsec-pb.log"
    grep Integrated\ weight\ \(pb\) ${dir}/${looking}/Events/mass_${i}GeV/mass_${i}GeV_tag_1_banner.txt | awk 'NR ==1' | awk '{print $6}' >> "DYL_nn23lo1_mass_xsec-pb.log"
done
