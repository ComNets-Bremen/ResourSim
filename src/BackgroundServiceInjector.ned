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
// Module which inject background service events
//
simple BackgroundServiceInjector
{
    parameters:
        @display("i=status/background-injector");
        int numBackgroundEvents = 2;

		// One service mimics the WiFi usage for WiFi
        volatile double backgroundEvent0_start @unit("s") 		= default(1s);
        volatile double backgroundEvent0_stop @unit("s") 		= default(200s);  // TODO: currently not used
        volatile double backgroundEvent0_repetition @unit("s") 	= default(uniform(550s, 650s));
        volatile double backgroundEvent0_duration @unit("s")	= default(truncnormal(2s, 10s));
        string backgroundEvent0_type @enum(BACKGROUND_EVENT_TYPE_CPU,BACKGROUND_EVENT_TYPE_WIFI,BACKGROUND_EVENT_TYPE_CELLULAR,BACKGROUND_EVENT_TYPE_BLUETOOTH) = default("BACKGROUND_EVENT_TYPE_WIFI");

		// One service processes data in the background
		volatile double backgroundEvent1_start @unit("s") 		= default(1s);
        volatile double backgroundEvent1_stop @unit("s") 		= default(200s);  // TODO: currently not used
        volatile double backgroundEvent1_repetition @unit("s") 	= default(normal(3600s, 60s));
        volatile double backgroundEvent1_duration @unit("s")	= default(truncnormal(2s, 5s));
        string backgroundEvent1_type @enum(BACKGROUND_EVENT_TYPE_CPU,BACKGROUND_EVENT_TYPE_WIFI,BACKGROUND_EVENT_TYPE_CELLULAR,BACKGROUND_EVENT_TYPE_BLUETOOTH) = default("BACKGROUND_EVENT_TYPE_CPU");
    gates:
        output out[] @loose;
}