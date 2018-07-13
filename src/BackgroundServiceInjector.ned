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
simple BackgroundServiceInjector
{
    parameters:
        @display("i=status/background-injector");
        int numBackgroundEvents = 1;

        volatile double backgroundEvent0_start @unit("s") 		= default(1s);
        volatile double backgroundEvent0_stop @unit("s") 		= default(200s);  // TODO: not used at the moment
        volatile double backgroundEvent0_repetition @unit("s") 	= uniform(5s, 400s);
        volatile double backgroundEvent0_duration @unit("s")	= truncnormal(5s, 10s);
        string backgroundEvent0_type @enum(BACKGROUND_EVENT_TYPE_CPU,BACKGROUND_EVENT_TYPE_WIFI,BACKGROUND_EVENT_TYPE_CELLULAR,BACKGROUND_EVENT_TYPE_BLUETOOTH) = default("BACKGROUND_EVENT_TYPE_WIFI");

    gates:
        output out[] @loose;
}