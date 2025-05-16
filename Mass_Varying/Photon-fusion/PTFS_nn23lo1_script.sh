#!/bin/bash
#SBATCH --qos=cu_hpc
#SBATCH --partition=cpu
#SBATCH --tasks-per-node=8
#SBATCH --mem-per-cpu=8G

shopt -s expand_aliases  # Enable alias expansion
source ~/.bashrc  # Source .bashrc

#define generate

pdf_name="nn23lo1"
config_name="ptfs_${pdf_name}"
pdf_id="230000"
current_dir=$(pwd)
gen_card="PTFS_nn23lo1_gen_card"
launch_card="PTFS_nn23lo1_launch_card"


echo -e "set lhapdf /work/app/lhapdf/6.5.4/bin/lhapdf-config\nimport model mono_spinzero" > $gen_card
echo -e "generate a a > mm+ mm-\noutput ${config_name}" >> $gen_card

echo "Finish_Gen_config"

MD $gen_card

#sed -i "s/include 'maxparticles.inc'/include '${current_dir}/${config_name}/Sources/maxparticles.inc'/g" "${current_dir}/${config_name}/Sources/genps.inc"

sed -i "s|include 'maxparticles.inc'|include '${current_dir}/${config_name}/Source/maxparticles.inc'|g" "${current_dir}/${config_name}/Source/genps.inc"

echo "Finish Editing_geps.inc" 
echo "$current_dir"

#define launch

echo -e "set lhapdf /work/app/lhapdf/6.5.4/bin/lhapdf-config\nlaunch ${config_name} -n mass_1000GeV" > $launch_card
echo -e "analysis=madanalysis5\nset run_card ebeam1 6500
set ebeam2 6500\nset lpp1 1\nset lpp2 1\nset nevents 10000\nset dynamical_scale_choice -1\nset fixed_couplings False\nset param_card mass 25 125\nset decay 4110000 0.000000e+0\nset gch 1 1.0" >> $launch_card
echo -e "set pdlabel nn23lo1\nset lhaid ${pdf_id}" >> $launch_card

#mono_mass="1000"
echo -e "set mass 4110000 1000\n" >> $launch_card

for (( i=1015; i<7001; i+=15 ))
do
   echo -e "launch ${config_name} -n mass_${i}GeV\nset mass 4110000 ${i}\n" >> $launch_card
done

echo "Finish writing_launch"
MD $launch_card


# colloect xsection
dir=$(pwd)
looking=$config_name
echo "" > "${looking}_mass_xsec-pb.log"
for (( i=1000;i<7001;i+=15))
do
    echo -n "${i} " >> "${looking}_info"
    tail -n 4 ${dir}/${looking}/Events/mass_${i}GeV/mass_${i}GeV_tag_1_banner.txt | awk 'NR ==1' | awk '{print $6}' >> "${looking}_info"
done

