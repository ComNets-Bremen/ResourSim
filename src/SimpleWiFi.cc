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

#include "SimpleWiFi.h"

namespace eventsimulator {

Define_Module(SimpleWiFi);

SimpleWiFi::SimpleWiFi(){
    wifiStatusValues.setName("WiFi Status");
}

SimpleWiFi::~SimpleWiFi(){

}

void SimpleWiFi::initialize()
{
    EV << "Init WiFi status" << endl;
    // TODO: Init something?
    initialized = true;
}

void SimpleWiFi::handleMessage(cMessage *msg)
{
    if (check_and_cast<BaseEventMessage *>(msg)->getPayloadType() != EVENT_TYPE_WIFI){
        // Not our message
        delete msg;
        return;
    }
    WiFiEventMessage *wifiMsg = check_and_cast<WiFiEventMessage *>(msg);
    wifiStatus = wifiMsg->getWifi_status();
    wifiStatusValues.record(wifiStatus);

    switch(wifiStatus){
        // TODO: Check if all offers internet connectivity
        case WIFI_CONNECTING:
        case WIFI_CONNECTED:
        case WIFI_AUTHENTICATING:
        case WIFI_OBTAINING_IPADDR:
        case WIFI_VERIFYING_POOR_LINK:
            wifiIsUsed = true;
            break;
        default:
            wifiIsUsed = false;
    }
    // TODO: Check if transition is valid?

    delete wifiMsg;
}

void SimpleWiFi::refreshDisplay() const {
    char buf[40];
    sprintf(buf, "WiFi status: %s", getWiFiStatusString(wifiStatus).c_str());
    getDisplayString().setTagArg("t", 0, buf);
    if (wifiIsUsed)
        getDisplayString().setTagArg("i", 0, "status/wifi_green");
    else
        getDisplayString().setTagArg("i", 0, "status/wifi_neutral");
}

} //namespace
