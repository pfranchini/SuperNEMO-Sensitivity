#!/bin/bash       

###################################################################################################
#
# Run multiple simulation on the IC batch system using /do_simulate.sh for each single bunch.
# Each simulation has the number of events specified in the conf file (usually 1E6~1day??).
#
# Usage: ./do_multiple.sh <configuration> <number of bunches>
#
###################################################################################################

###############################
TIME=1000 
###############################

CONFIGURATION=${1} # configuration file
N=${2}             # number of bunches

# Batch standard output/ standard error files
mkdir -p logs

# Purge old conf and seed files
rm -f ${CONFIGURATION}-?.conf
rm -f ${CONFIGURATION}-??.conf
rm -f ${CONFIGURATION}-?.sh
rm -f ${CONFIGURATION}-??.sh
rm -f ${CONFIGURATION}-seed*.conf

# Falaise from Singularity
shopt -s expand_aliases
alias snemo-sing='singularity exec --home /home/hep/pfranchi --bind /vols/snemo/ -c /vols/build/snemo/falaise/falaise_latest.sif'

# Check if the configuration files exists
if [ ! -f "${CONFIGURATION}.conf" ]; then
    echo "Wrong configuration file ${CONFIGURATION}.conf!!!"
    exit
fi

# Wait for the next full minute so all the bunches will eventually end up in the same folder YYYYMMDD-hhmm
echo "Wait for the next full minute..."
sleep $((60 - $(date +%S) ))

# Loop for single simulations
for i in `seq 1 $N`; do

    cp $CONFIGURATION.conf $CONFIGURATION-${i}.conf

    # Randmon seeds generated inside a file as falaise does
    snemo-sing bxg4_seeds -k -T -d ${PWD} -p ${CONFIGURATION}-seed${i}.conf
    echo "[name=\"flsimulate.simulation\" type=\"flsimulate::section\"]" >> ${CONFIGURATION}-${i}.conf
    echo "    rngSeedFile : string as path = \"${PWD}/$CONFIGURATION-seed${i}.conf\"" >> ${CONFIGURATION}-${i}.conf

    # Interactive:    
    #./do_simulate.sh ${CONFIGURATION}-${i} &

    # Batch system:
    echo "date; ./do_simulate.sh ${CONFIGURATION}-${i}; rm $CONFIGURATION-${i}.conf; rm ${CONFIGURATION}-seed${i}.conf; date" > ${CONFIGURATION}-${i}.sh
    qsub -q hep.q -l h_rt=${TIME} -cwd -e ${PWD}/logs -o ${PWD}/logs ${CONFIGURATION}-${i}.sh
    sleep 1
    rm -f ${CONFIGURATION}-${i}.sh
    
done

sleep 10
if [ `qstat | grep ${USER} | wc -l` -lt "$N" ]; then
    echo "Only "`qstat | grep ${USER} | wc -l`" jobs in the queue, please check if anything went wrong!"
fi

echo "--> Output jobs in:" ${PWD}/`ls -dtr 2*/ | tail -n1`

