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

package eventsimulator.simulations;

import eventsimulator.PhoneEventInjector;
import eventsimulator.EventManager;
import eventsimulator.SimplePercentageBattery;
import eventsimulator.SimpleScreen;
import eventsimulator.SimpleAirplaneMode;
import eventsimulator.SimpleWiFi;


network EventNetwork
{
    @display("bgb=606,470");
    submodules:
        userEventInjector[1]: PhoneEventInjector {
            @display("p=82,46");
        }
        manager[1]: EventManager {
            @display("p=277,169");
        }
        battery[1]: SimplePercentageBattery {
            @display("p=483,186");
        }
        screen[1]: SimpleScreen {
            @display("p=286,40");
        }
        airplane[1]: SimpleAirplaneMode {
            @display("p=385,326");
        }
        wifi[1]: SimpleWiFi {

            @display("p=179,310");
        }
    connections:
        userEventInjector[0].out++ --> manager[0].in++;
        manager[0].out++ --> battery[0].in++;
        manager[0].out++ --> screen[0].in++;
        manager[0].out++ --> airplane[0].in++;
        manager[0].out++ --> wifi[0].in++;
}
