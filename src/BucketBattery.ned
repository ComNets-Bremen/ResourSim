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
// TODO auto-generated module
//
simple BucketBattery
{
    parameters:
        volatile double inconvenientBatteryThreshold = default(0.3);
        volatile int periodicStatsCollectionInterval @unit("s") = default(240s);

        // 1As = 1 C
        // Example battery (Samsung Galaxy S4 mini:
        // 9.88 Wh @ 3.7V = 2670 mAh = 9613 C
        // 9.88 / 3.7 *60*60 = 9613
        volatile double batteryCapacityCoulomb = default(9613);

        // ~11.3 day discharging w/o any usage = 271.2 h = 976320 s
        // 9613 C total capacity in 272.2 hrs = 35.3 C/h average discharge
        volatile double batteryDischargeCoulombPerHour = default(35.32);

        // ~3hrs charging
        // 9613 C total capacity in 3 hrs = 3204 C/h average charge
        volatile double batteryChargeCoulombPerHour = default(3203)
        @display("i=status/battery_neutral");

    gates:
        input in[];
        output out[];
}
