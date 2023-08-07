#!/usr/bin/env sh

# Run the general configuration using the command line environment and redirect
# the output to a logfile

sims=$(grep "\[*\]" simulations/omnetpp.ini)
simulations=$(echo $sims | sed "s/Config //g" | sed "s/\] \[/\]\n\[/g" | nl)
#echo $simulations

#simulations=$(grep "\[*\]" simulations/omnetpp.ini | sed "s/Config //g" | nl)

item=$(dialog --menu --output-fd 1 "Select simulation" 0 0 0 $simulations)
echo $item


#SIMTYPE="BucketBattery-WiFi-CPU-Background"
SIMTYPE="AllUserSims"
#SIMTYPE="WindowOptimization"
#SIMTYPE="TestUser1"
#cd simulations
#../Resoursim -m -u Cmdenv -c $SIMTYPE -n .:../src --image-path=../images omnetpp.ini

#opp_runall -j 2 ../Resoursim -m -u Cmdenv -c $SIMTYPE -n .:../src --image-path=../images omnetpp.ini


