//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package eventsimulator;

// Simple EventManager. Currently, it sends all incoming messages to all out
// ports. The receivers have to decide whether they handle the message or not

simple EventManager
{
    parameters:
        //@display("i=block/dispatch");
        @display("i=status/status_okay");
        // Stop forwarding messages if device is dead
        volatile bool stopForwardingDeviceDead = default(false);
        
        volatile int sendBatteryCollectSignalEvent @unit("s") = default(240s);
        
        // Collect data for graphing
        volatile int collectDecisionDatasets @unit("s") = default(600s);
        
        // General optimizations
        volatile bool enableBackgroundOptimizations = default(false);
        volatile bool ignoreBackgroundEventsBatteryInconvenient = default(false);
        volatile bool analyzeUserScreenActivity = default(false);
        volatile bool analyzeTimeToCharge = default(false);
        volatile double chargeDecisionQuantil = default(0.5); // median
        volatile int analyzeTimeToChargeMinDataLen = default(10); // Minimum number of dataset to use this optimization technique
        
        volatile double screenCorrectionFactor = default(1);
        
        // Use the average window for the decision
        volatile int decisionWindow @unit("s") = default(1800s);
        
        volatile string writeScreenOffTimes = default("");
        volatile string writeTimesBetweenChg = default("");
    gates:
        input in[];
        output out[];
}
