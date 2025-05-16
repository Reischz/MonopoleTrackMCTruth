#!/bin/bash
#SBATCH --qos=cu_hpc
#SBATCH --partition=cpu
#SBATCH --tasks-per-node=8
#SBATCH --mem-per-cpu=8G

pvn="0"
#rm -rf UniqueDeTc.log
for i in ../../fittingParameterDistribution_fiding/GEN*.log
do
    echo "Processing $i"
    # # Extract the line number of the Particle = line
    root "FindUnique.cpp(\"$i\")"
    # more $i | grep -n Particle\ = > temp.txt
    # # Use : as the delimiter in awk
    # more temp.txt | awk -F':' '{print $1}' > temp2.txt
    # # create command via cpp
    # root "CreCom.cpp(\"temp2.txt\",\"$i\")"
    # while IFS= read -r line
    # do
    #     # line tructure from cpp sed -n '2361,57962p' GENSIM_2018_2000_2_SpinHalf_Premix.log
    #     number=$(eval $line | awk '{print $18}' | awk '!/[0-9]/' | awk '!seen[$0]++' | wc -l)
    #     if [ $pvn -gt $number ]
    #     then
    #         pvn=$number
    #         echo "Number of DEtector Apperance: $number"
    #         eval $line > ThisHierarchy.log
    #     fi
    # done < "temp3.txt"
done
