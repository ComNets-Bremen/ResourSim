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

// A message type representing the traffic on a mobile phone

cplusplus {{
#include "BaseEventMessage_m.h"

struct TrafficEventValues{
    long mobile_rx;
    long mobile_tx;
    long total_rx;
    long total_tx;
    omnetpp::simtime_t received;
};
}}
message BaseEventMessage;

message TrafficEventMessage extends BaseEventMessage {
    payloadType=EVENT_TYPE_TRAFFIC;
    long mobile_rx;
    long mobile_tx;
    long total_rx;
    long total_tx;
}
