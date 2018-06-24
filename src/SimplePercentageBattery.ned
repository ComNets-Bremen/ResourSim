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

//
simple SimplePercentageBattery
{
    parameters:
        // Median values, taken from real data. Replace by distributions for more realistic values
        volatile double chargePerHour = default(0.6563156064077952);		// percent per hour
        volatile double dischargePerHour = default(-0.12693461937441633);	// percent per hour
        @display("i=status/battery_neutral");
    gates:
        input in[];
        output out[];
}
