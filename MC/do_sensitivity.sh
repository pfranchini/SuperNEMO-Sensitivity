#!/bin/bash

# Run the sensitivity module on a reconstructed brio file
#
# Usage: ./do_simulate.sh <reconstructed_file> (omitt .brio extention)
# Output: <reconstructed_file>.root

###############################

SENSITIVITY="$HOME/SuperNEMO/SensitivityModuleBuild"

###############################

shopt -s expand_aliases
alias snemo-sing='singularity exec --home /home/hep/pfranchi --bind /vols/build/snemo/ -c /vols/build/snemo/falaise/falaise_latest.sif'


FILE=${1}
if [ ! -f "${FILE}.brio" ]; then
    echo "Wrong brio file ${FILE}.brio!!!"
    exit
fi

# Create the local directory
#WHERE=$PWD/`date "+%Y%m%d-%H%M"`
#mkdir -p $WHERE
#rm -f $WHERE/$CONFIGURATION.log

# Create Sensitivity conf with the proper filename
cp ${SENSITIVITY}/SensitivityModuleExample.conf $FILE-sensitivity.conf
echo "filename_out : string[1] = \"$FILE.root\"" >> $FILE-sensitivity.conf

echo "Using: " ${FILE}.brio
#echo "Working in: " $WHERE

echo "Sensitivity module..."
echo "snemo-sing flreconstruct -i $FILE.brio -p $FILE-sensitivity.conf -o $FILE-sim.root"
snemo-sing flreconstruct -i $FILE.brio -p $FILE-sensitivity.conf -o $FILE-sim.root &>> $FILE.log

echo "--> Output from Sensitivity in: "$FILE.root
