
# Usage:
# ./do_simulate_multiple.sh "<conf1> <conf2> ... <conf3>"
# e.g.: ./do_simulate_multiple.sh "bi214 radon"

LIST=$1

echo "I will simulate:"

for CONF in $LIST
do
    echo "  " $CONF.conf
done

for CONF in $LIST
do
    echo $CONF
    ./do_simulate.sh $CONF &
done
