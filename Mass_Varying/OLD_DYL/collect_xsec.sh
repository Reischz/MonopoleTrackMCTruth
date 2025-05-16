#!/bin/bash
#SBATCH --qos=cu_hpc
#SBATCH --partition=cpu
#SBATCH --tasks-per-node=8
#SBATCH --mem-per-cpu=8G

# colloect xsection
dir=$(pwd)
looking="DYL_LUXqed_plus_PDF4LHC15_nnlo_100"
rm -rf "${looking}_mass_xsec-pb.log"
for (( i=1000;i<5001;i+=15))
do
    echo -n "${i} " >> "${looking}_mass_xsec-pb.log"
    grep Integrated\ weight\ \(pb\) ${dir}/${looking}/Events/mass_${i}GeV/mass_${i}GeV_tag_1_banner.txt | awk 'NR ==1' | awk '{print $6}' >> "${looking}_mass_xsec-pb.log"
done

# colloect xsection
looking="DYL_nn23lo1"
rm -rf "${looking}_mass_xsec-pb.log"
for (( i=1000;i<5001;i+=15))
do
    echo -n "${i} " >> "${looking}_mass_xsec-pb.log"
    grep Integrated\ weight\ \(pb\) ${dir}/${looking}/Events/mass_${i}GeV/mass_${i}GeV_tag_1_banner.txt | awk 'NR ==1' | awk '{print $6}' >> "${looking}_mass_xsec-pb.log"
done

# colloect xsection
looking="DYL_NNPDF31_nlo_as_0118_luxqed"
rm -rf "${looking}_mass_xsec-pb.log"
for (( i=1000;i<5001;i+=15))
do
    echo -n "${i} " >> "${looking}_mass_xsec-pb.log"
    grep Integrated\ weight\ \(pb\) ${dir}/${looking}/Events/mass_${i}GeV/mass_${i}GeV_tag_1_banner.txt | awk 'NR ==1' | awk '{print $6}' >> "${looking}_mass_xsec-pb.log"
done




