#!/bin/bash

##############################################################################
#
# Create a sub-directory YYYYMMDD-hhmm"
# Execute a full SNEMO/Falaise simulation-->reconstruction-->sensitivity
#
# Usage: ./do_simulate.sh <configuration>
# Output: configuration.root
#
# Can use binaries from a built version of Falaise or a Docker image.
#
# From Falaise: the resultant file <configuration>-reco.root contains
# a single flat TTree structure that may be browsed interactively.
# Note that currently this format only outputs simulated and calibrated data,
# with no further reconstruction results.
#
##############################################################################

PURGE=false                                                                     # delete sim and reco files to save space
SENSITIVITY="$HOME/SuperNEMO/SensitivityModuleBuild"                            # location of the SensitivityModule
RECO_CONFIG="@falaise:snemo/demonstrator/reconstruction/official-2.0.0.conf"    # reconstruction config file
#FALAISE="/vols/build/snemo/Falaise.fork.build/bin/"                             # Falaise software
FALAISE="singularity exec --home /home/hep/pfranchi --bind /vols/snemo/ -c /vols/build/snemo/falaise/falaise_latest.sif "

##############################################################################

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

${FALAISE}flsimulate --version >> $WHERE/$CONFIGURATION.log
echo "${CONFIGURATION}.conf" >> $WHERE/$CONFIGURATION.log
cat ${CONFIGURATION}.conf >> $WHERE/$CONFIGURATION.log

# Create Sensitivity conf with the proper filename
cp ${SENSITIVITY}/SensitivityModuleExample.conf $WHERE/$CONFIGURATION-sensitivity.conf
echo "filename_out : string[1] = \"$WHERE/$CONFIGURATION.root\"" >> $WHERE/$CONFIGURATION-sensitivity.conf

echo "Using: " ${CONFIGURATION}.conf
echo "Working in: " $WHERE

echo "Simulation..."
${FALAISE}flsimulate -c $PWD/$CONFIGURATION.conf -o $WHERE/$CONFIGURATION-sim.brio &>> $WHERE/$CONFIGURATION.log
#echo "${FALAISE}flsimulate -c $PWD/$CONFIGURATION.conf -o $WHERE/$CONFIGURATION-sim.brio"

if [ ! -f "$WHERE/$CONFIGURATION-sim.brio" ]; then
    echo -e "Simulation file not present, something wrong with flsimulate!!!\n"
    tail -n +12 $WHERE/$CONFIGURATION.log
    exit
fi

echo "Reconstruction..."
${FALAISE}flreconstruct -i $WHERE/$CONFIGURATION-sim.brio -p ${RECO_CONFIG} -o $WHERE/$CONFIGURATION-reco.brio &>> $WHERE/$CONFIGURATION.log
# Remove unused filed to save space
if [ "$PURGE" = true ] ; then
    rm -f $WHERE/$CONFIGURATION-sim.brio
fi

if [ ! -f "$WHERE/$CONFIGURATION-reco.brio" ]; then
    echo "Reconstruction file not present, something wrong with flreconstruct!!!"
    exit
fi

echo "Sensitivity module..."
snemo-sing flreconstruct -i $WHERE/$CONFIGURATION-reco.brio -p $WHERE/$CONFIGURATION-sensitivity.conf -o $WHERE/$CONFIGURATION-reco.root &>> $WHERE/$CONFIGURATION.log

if [ ! -f "$WHERE/$CONFIGURATION.root" ]; then
    echo "Output from Sensitivity not present, something wrong with flreconstruct and Sensitivity module!!!"
    exit
fi

# Remove unused filed to save space   
if [ "$PURGE" = true ] ; then
    rm -f $WHERE/$CONFIGURATION-reco.brio
    rm -f $WHERE/$CONFIGURATION-reco.root
fi

echo "--> Output from Sensitivity in: "$WHERE/$CONFIGURATION.root
