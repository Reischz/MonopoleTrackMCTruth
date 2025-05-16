#!bin/bash

dir=$(pwd)
looking="ptfs_LUXqed_plus_PDF4LHC15_nnlo_100"
echo "" > "${looking}_info"
for (( i=1001;i<2027;i+=5))
do
    echo -n "${i} " >> "${looking}_info"
    tail -n 4 ${dir}/${looking}/Events/mass_${i}GeV/mass_${i}GeV_tag_1_banner.txt | awk 'NR ==1' | awk '{print $6}' >> "${looking}_info"
done


