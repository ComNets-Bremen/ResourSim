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

import eventsimulator.Txc;
import eventsimulator.PhoneEventInjector;

//
// Two instances (tic and toc) of Txc connected.
//
network Tictoc
{
    @display("bgb=655,436");
    submodules:
        tic: Txc {
            @display("p=129,98");
        }
        toc: Txc {
            @display("p=90,321");
        }
        inj: PhoneEventInjector {
            @display("p=403,166");
        }
    connections:
        tic.out --> toc.in;
        tic.in <-- toc.out;
        //inj.out --> toc.in;
}
