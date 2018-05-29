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

#include "EventReceiver.h"

namespace eventsimulator {

Define_Module(EventReceiver);

void EventReceiver::initialize()
{
    EV_INFO << "Started" << endl;
}

void EventReceiver::handleMessage(cMessage *msg)
{

    BaseEventMessage *message = check_and_cast<BaseEventMessage *>(msg);
    EV_INFO << "Message type: " << message->getPayloadType() << endl;

    switch (message->getPayloadType())
    {
        case EVENT_TYPE_SCREEN:
            //EV_INFO << "EVENT_TYPE_SCREEN" << endl;
            handleScreenEvent(check_and_cast<ScreenEventMessage *>(msg));
            break;
        case EVENT_TYPE_BATTERY:
            //EV_INFO << "EVENT_TYPE_BATTERY" << endl;
            handleBatteryEvent(check_and_cast<BatteryEventMessage *>(msg));
            break;
        case EVENT_TYPE_WIFI:
            //EV_INFO << "EVENT_TYPE_WIFI" << endl;
            handleWiFiEvent(check_and_cast<WiFiEventMessage *>(msg));
            break;
        case EVENT_TYPE_TRAFFIC:
            //EV_INFO << "EVENT_TYPE_TRAFFIC" << endl;
            handleTrafficEvent(check_and_cast<TrafficEventMessage *>(msg));
            break;
        case EVENT_TYPE_CELLULAR:
            //EV_INFO << "EVENT_TYPE_CELLULAR" << endl;
            handleCellularEvent(check_and_cast<CellularEventMessage *>(msg));
            break;
        case EVENT_TYPE_AIRPLANE_MODE:
            //EV_INFO << "EVENT_TYPE_AIRPLANE_MODE" << endl;
            handleAirplaneModeEvent(check_and_cast<AirplaneModeEventMessage *>(msg));
            break;
        case EVENT_TYPE_BLUETOOTH:
            //EV_INFO << "EVENT_TYPE_BLUETOOTH" << endl;
            handleBluetoothEvent(check_and_cast<BluetoothEventMessage *>(msg));
            break;
        case EVENT_TYPE_UNKNOWN:
            // Just the base event
            //EV_INFO << "EVENT_TYPE_UNKNOWN" << endl;
            handleUnknownEvent(message);
            delete msg;
            break;
        default:
            EV_ERROR << "Undefined Event" << endl;
            delete msg;
    }


}

void EventReceiver::handleScreenEvent(ScreenEventMessage *msg){
    EV_INFO << "@" << simTime() << " Screen Event: Screen on: " << msg->getScreenOn() << endl;
    delete msg;
}

void EventReceiver::handleBatteryEvent(BatteryEventMessage *msg){
    EV_INFO << "@" << simTime() << " Battery Event: percentage: " << msg->getPercentage() << " chg_ac: " << msg->getChg_ac() << " chg_usb: " << msg->getChg_usb() << " chg_wireless: " << msg->getChg_wireless() << " is_charging: " << msg->getIs_charging() << " absPercentage: " << msg->getTheoreticalAbsolutePercentage() << endl;
    delete msg;
}

void EventReceiver::handleWiFiEvent(WiFiEventMessage *msg){
    EV_INFO << "@" << simTime() << " WiFi Event: WiFi Status: " << getWiFiStatusString(msg->getWifi_status()) << endl;
    delete msg;
}

void EventReceiver::handleTrafficEvent(TrafficEventMessage *msg){
    EV_INFO << "@" << simTime() << " Traffic Event: mobile_rx: " << msg->getMobile_rx() << " mobile_tx: " << msg->getMobile_tx() << " total_rx: " << msg->getTotal_rx() << " total_tx: " << msg->getTotal_tx() << endl;
    delete msg;
}

void EventReceiver::handleCellularEvent(CellularEventMessage *msg){
    EV_INFO << "@" << simTime() << " Cellular Event: Cellular state: " << getCellularStateString(msg->getCellular_state()) << " type: " << msg->getCellular_type() << endl;
    delete msg;
}

void EventReceiver::handleAirplaneModeEvent(AirplaneModeEventMessage *msg){
    EV_INFO << "@" << simTime() << " Airplane Mode Event: airplane mode on: " << msg->getAirplaneModeOn() << endl;
    delete msg;
}

void EventReceiver::handleBluetoothEvent(BluetoothEventMessage *msg){
    EV_INFO << "@" << simTime() << " Bluetooth Event: Bluetooth Status: " << getBluetoothStatusString(msg->getBluetooth_status()) << endl;
    delete msg;
}

void EventReceiver::handleUnknownEvent(BaseEventMessage *msg){
    EV_ERROR << "@" << simTime() << " Event is not defined!" << msg << endl;
    delete msg;
}
}; // namespace
