#!/usr/bin/env sh

# Run the general configuration using the command line environment and redirect
# the output to a logfile

#SIMTYPE="BucketBattery-WiFi-CPU-Background"
SIMTYPE="AllUserSims"
#SIMTYPE="WindowOptimization"
#SIMTYPE="TestUser1"
cd simulations
#../Resoursim -m -u Cmdenv -c $SIMTYPE -n .:../src --image-path=../images omnetpp.ini

opp_runall -j 2 ../Resoursim -m -u Cmdenv -c $SIMTYPE -n .:../src --image-path=../images omnetpp.ini


