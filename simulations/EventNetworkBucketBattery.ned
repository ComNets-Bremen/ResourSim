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
import eventsimulator.BucketBattery;
import eventsimulator.SimpleScreen;
import eventsimulator.SimpleAirplaneMode;
import eventsimulator.SimpleWiFi;
import eventsimulator.SimpleCellular;
import eventsimulator.SimpleBluetooth;
import eventsimulator.BackgroundServiceInjector;
import eventsimulator.SimpleHighPowerCpu;


network EventNetworkBucketBattery
{
    @display("bgb=546,382");
    submodules:
        userEventInjector[1]: PhoneEventInjector {
            @display("p=79,47");
        }

        backgroundServiceInjector[1]: BackgroundServiceInjector {
            @display("p=108,124");
        }

        manager[1]: EventManager {
            @display("p=286,169");
        }

        battery[1]: BucketBattery {
            @display("p=483,186");
        }

        screen[1]: SimpleScreen {
            @display("p=286,47");
        }

        airplane[1]: SimpleAirplaneMode {
            @display("p=428,277");
        }

        wifi[1]: SimpleWiFi {
            @display("p=286,310");
        }

        cellular[1]: SimpleCellular {
            @display("p=460,85");
        }

        bluetooth[1]: SimpleBluetooth {
            @display("p=148,277");
        }

        cpu[1]: SimpleHighPowerCpu {

            @display("p=53,218");
        }
    connections:
        userEventInjector[0].out++ --> manager[0].in++;
        backgroundServiceInjector[0].out++ --> manager[0].in++;
        manager[0].out++ --> battery[0].in++;
        manager[0].out++ --> screen[0].in++;
        manager[0].out++ --> airplane[0].in++;
        manager[0].out++ --> wifi[0].in++;
        manager[0].out++ --> cellular[0].in++;
        manager[0].out++ --> bluetooth[0].in++;
        manager[0].out++ --> cpu[0].in++;

        wifi[0].out++ --> manager[0].in++;
        battery[0].out++ --> manager[0].in++;
        screen[0].out++ --> manager[0].in++;
        cellular[0].out++ --> manager[0].in++;
        bluetooth[0].out++ --> manager[0].in++;
        airplane[0].out++ --> manager[0].in++;
        cpu[0].out++ --> manager[0].in++;
}
