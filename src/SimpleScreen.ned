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


// This module represents the status of the screen

package eventsimulator;

simple SimpleScreen
{
    parameters:
    	@display("i=status/screen_off");
    	volatile int statsWindowSize @unit("s")	= default(3600s);
    	volatile int periodicStatsCollectionInterval @unit("s") = default(240s);
    	
    	volatile double screenCurrentDrawn = default(0.150); // 150 mA, TODO: check
    gates:
        input in[];
        output out[];
}
