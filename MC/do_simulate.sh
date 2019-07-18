#!/bin/bash

# Create a sub-directory YYYYMMDD-hhmm"
# Execute a full SNEMO/Falaise simulation-->reconstruction-->sentitivity
#
# Usage: ./do_simulate.sh <configuration>
# Output: configuration.root

###############################

SENSITIVITY="$HOME/SuperNEMO/SensitivityModuleBuild"

###############################

shopt -s expand_aliases
alias snemo-sing='singularity exec --home /home/hep/pfranchi --bind /vols/build/snemo/ -C /vols/build/snemo/falaise/falaise_latest.sif'

CONFIGURATION=${1}
if [ ! -f "${CONFIGURATION}.conf" ]; then
    echo "Wrong configuration file ${CONFIGURATION}.conf!!!"
    exit
fi

# Create the local directory
WHERE=$PWD/`date "+%Y%m%d-%H%M"`
mkdir -p $WHERE
rm -f $WHERE/$CONFIGURATION.log

# Create Sensitivity conf with the proper filename
cp ${SENSITIVITY}/SensitivityModuleExample.conf $WHERE/$CONFIGURATION-sensitivity.conf
echo "filename_out : string[1] = \"$WHERE/$CONFIGURATION.root\"" >> $WHERE/$CONFIGURATION-sensitivity.conf

echo "Using: " ${CONFIGURATION}.conf
echo "Working in: " $WHERE

echo "Simulation..."
snemo-sing flsimulate -c $PWD/$CONFIGURATION.conf -o $WHERE/$CONFIGURATION-sim.brio &>> $WHERE/$CONFIGURATION.log

echo "Reconstruction..."
snemo-sing flreconstruct -i $WHERE/$CONFIGURATION-sim.brio -p @falaise:snemo/demonstrator/reconstruction/official-2.0.0.conf -o $WHERE/$CONFIGURATION-reco.brio &>> $WHERE/$CONFIGURATION.log

echo "Sensitivity module..."
snemo-sing flreconstruct -i $WHERE/$CONFIGURATION-reco.brio -p $WHERE/$CONFIGURATION-sensitivity.conf -o $WHERE/$CONFIGURATION-sim.root &>> $WHERE/$CONFIGURATION.log

echo "--> Output from Sensitivity in: "$WHERE/$CONFIGURATION.root
