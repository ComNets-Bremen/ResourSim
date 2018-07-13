#!/usr/bin/env sh

# Run the general configuration using the command line environment and redirect
# the output to a logfile

cd simulations
../Resoursim -m -u Cmdenv -n .:../src --image-path=../images omnetpp.ini
