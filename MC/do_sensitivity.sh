#!/bin/bash

# Run the sensitivity module on a reconstructed brio file
#
# Usage: ./do_simulate.sh <reconstructed_file> (omit .brio extension)
# Output: <reconstructed_file>.root

#######################################################################

#SENSITIVITY="$HOME/SuperNEMO/SensitivityModuleBuild"
SENSITIVITY="$HOME/SuperNEMO/SensitivityModule.fork.build"
#FALAISE="/vols/build/snemo/Falaise.build/bin/"  
FALAISE="singularity exec --home /home/hep/pfranchi --bind /vols/snemo/ -c /vols/build/snemo/falaise/falaise_latest.sif "   

#######################################################################

FILE=${1}
WHERE=$PWD

shopt -s expand_aliases
alias snemo-sing='singularity exec --home /home/hep/pfranchi --bind /vols/build/snemo/ -c /vols/build/snemo/falaise/falaise_latest.sif'

if [ ! -f "${FILE}.brio" ]; then
    echo "Wrong brio file ${FILE}.brio!!!"
    exit
fi

# Remove eventual existing output file from a previous Sensitivity module run
rm -f $WHERE/$FILE.root

# Create the local directory
#WHERE=$PWD/`date "+%Y%m%d-%H%M"`
#mkdir -p $WHERE
#rm -f $WHERE/$CONFIGURATION.log

# Create Sensitivity conf with the proper filename
cp ${SENSITIVITY}/SensitivityModuleExample.conf $FILE-sensitivity.conf
echo "filename_out : string[1] = \"$WHERE/$FILE.root\"" >> $FILE-sensitivity.conf

echo "Using: " ${FILE}.brio
echo "Working in: " $WHERE

echo "Sensitivity module..."
echo "snemo-sing flreconstruct -i ${FILE}.brio -p $FILE-sensitivity.conf -o $FILE-sim.root"
${FALAISE}flreconstruct -i $WHERE/$FILE.brio -p $WHERE/$FILE-sensitivity.conf -o $WHERE/$FILE-sim.root &> $FILE.log

if [ ! -f "$WHERE/$FILE.root" ]; then
    echo "Output from Sensitivity not present, something wrong with flreconstruct and Sensitivity module!!!"
    exit
fi


rm -f $WHERE/$FILE-sim.root

echo "--> Output from Sensitivity in: "$WHERE/$FILE.root
