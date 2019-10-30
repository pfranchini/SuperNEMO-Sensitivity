#!/bin/bash

##############################################################################
#
# Create a sub-directory YYYYMMDD-hhmm"
# Execute a full SNEMO/Falaise simulation-->reconstruction-->sentitivity
#
# Usage: ./do_simulate.sh <configuration>
# Output: configuration.root
#
# From Falaise: the resultant file <configuration>-reco.root contains
# a single flat TTree structure that may be browsed interactively.
# Note that currently this format only outputs simulated and calibrated data,
# with no further reconstruction results.
#
##############################################################################

SENSITIVITY="$HOME/SuperNEMO/SensitivityModuleBuild"

###############################

shopt -s expand_aliases
alias snemo-sing='singularity exec --home /home/hep/pfranchi --bind /vols/snemo/ -c /vols/build/snemo/falaise/falaise_latest.sif'

CONFIGURATION=${1}
if [ ! -f "${CONFIGURATION}.conf" ]; then
    echo "Wrong configuration file ${CONFIGURATION}.conf!!!"
    exit
fi

# Create the local directory
WHERE=$PWD/`date "+%Y%m%d-%H%M"`
mkdir -p $WHERE
rm -f $WHERE/$CONFIGURATION.log

snemo-sing flsimulate --version >> $WHERE/$CONFIGURATION.log

# Create Sensitivity conf with the proper filename
cp ${SENSITIVITY}/SensitivityModuleExample.conf $WHERE/$CONFIGURATION-sensitivity.conf
echo "filename_out : string[1] = \"$WHERE/$CONFIGURATION.root\"" >> $WHERE/$CONFIGURATION-sensitivity.conf

echo "Using: " ${CONFIGURATION}.conf
echo "Working in: " $WHERE

echo "Simulation..."
snemo-sing flsimulate -c $PWD/$CONFIGURATION.conf -o $WHERE/$CONFIGURATION-sim.brio &>> $WHERE/$CONFIGURATION.log
echo "snemo-sing flsimulate -c $PWD/$CONFIGURATION.conf -o $WHERE/$CONFIGURATION-sim.brio"

if [ ! -f "$WHERE/$CONFIGURATION-sim.brio" ]; then
    echo "Simulation not present, something wrong with flsimulate!!!"
    exit
fi

echo "Reconstruction..."
snemo-sing flreconstruct -i $WHERE/$CONFIGURATION-sim.brio -p @falaise:snemo/demonstrator/reconstruction/official-2.0.0.conf -o $WHERE/$CONFIGURATION-reco.brio &>> $WHERE/$CONFIGURATION.log
# Remove unused filed to save space
rm -f $WHERE/$CONFIGURATION-sim.brio

if [ ! -f "$WHERE/$CONFIGURATION-reco.brio" ]; then
    echo "Reconstruction not present, something wrong with flreconstruct!!!"
    exit
fi

echo "Sensitivity module..."
snemo-sing flreconstruct -i $WHERE/$CONFIGURATION-reco.brio -p $WHERE/$CONFIGURATION-sensitivity.conf -o $WHERE/$CONFIGURATION-reco.root &>> $WHERE/$CONFIGURATION.log

if [ ! -f "$WHERE/$CONFIGURATION.root" ]; then
    echo "Output from Sensitivity not present, something wrong with flreconstruct and Sensitivity module!!!"
    exit
fi

# Remove unused filed to save space   
rm -f $WHERE/$CONFIGURATION-reco.brio
rm -f $WHERE/$CONFIGURATION-reco.root

echo "--> Output from Sensitivity in: "$WHERE/$CONFIGURATION.root
