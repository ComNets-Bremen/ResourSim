#!/usr/bin/env sh

# Run the general configuration using the command line environment and redirect
# the output to a logfile

#SIMTYPE="BucketBattery-WiFi-CPU-Background"
SIMTYPE="UserOptimizationUser1"
cd simulations
../Resoursim -m -u Cmdenv -c $SIMTYPE -n .:../src --image-path=../images omnetpp.ini
