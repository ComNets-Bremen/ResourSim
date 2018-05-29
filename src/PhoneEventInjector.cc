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

#include "PhoneEventInjector.h"
#include "event_messages/EventMessages.h"

#include <vector>
#include <string>
#include <algorithm>

#include "utils.h"

namespace eventsimulator {

Define_Module(PhoneEventInjector);

typedef std::pair<simtime_t, BaseEventMessage*> eventPair;

std::list<eventPair> eventList;

bool sortEventList(const eventPair &a, const eventPair &b){
    return a.first < b.first;
}

void PhoneEventInjector::initialize()
{
    cXMLElement *xmlEvents = par("eventFilename").xmlValue();
    std::vector<std::string> unhandledEvents;

    if (xmlEvents == nullptr)
        throw cRuntimeError("No event xml file");

    cXMLElementList allEvents = xmlEvents->getElementsByTagName("event");

    for (auto const& value:allEvents){

        /*
        // Print Attributes
        EV_INFO << "Attributes: ";
        auto attrs = value->getAttributes();
        for (cXMLAttributeMap::iterator it = attrs.begin(); it != attrs.end(); ++it){
            EV_INFO << it->first << ", ";
        }
        EV_INFO << endl;
        */
        auto eventType = value->getAttribute("data_type");
        if (eventType == nullptr) {
            EV_ERROR << "Can not identify data type. No \"data_type\" attribute. Skipping..." << endl;;
            continue;
        }

        auto eventTime = value->getAttribute("timestamp_s");
        if (eventTime == nullptr){
            EV_ERROR << "No event time!" << endl;
            continue;
        }

        simtime_t eventTimestamp = SimTime(convertToDouble(eventTime)*1000.0, SimTimeUnit::SIMTIME_MS);

        if (strcmp(eventType, "ScreenStatus") == 0){
            //EV_INFO << "Handling " << eventType << "..." << endl;
            ScreenEventMessage *msg = new ScreenEventMessage(eventType);
            msg->setScreenOn(convertToBool(value->getAttribute("screen_status")));
            eventList.push_back(eventPair(eventTimestamp, msg));
        } else if (strcmp(eventType, "TrafficStatistics") == 0) {
            //EV_INFO << "Handling " << eventType << "..." << endl;
            TrafficEventMessage *msg = new TrafficEventMessage(eventType);
            msg->setTotal_rx(convertToLong(value->getAttribute("total_rx")));
            msg->setTotal_tx(convertToLong(value->getAttribute("total_tx")));
            msg->setMobile_rx(convertToLong(value->getAttribute("mobile_rx")));
            msg->setMobile_tx(convertToLong(value->getAttribute("mobile_tx")));
            eventList.push_back(eventPair(eventTimestamp, msg));
        } else if (strcmp(eventType, "BatteryStatus") == 0) {
            //EV_INFO << "Handling " << eventType << "..." << endl;
            BatteryEventMessage *msg = new BatteryEventMessage(eventType);
            msg->setPercentage(convertToDouble(value->getAttribute("delta_percentage")));
            msg->setTheoreticalAbsolutePercentage(convertToDouble(value->getAttribute("percentage")));
            msg->setChg_ac(convertToBool(value->getAttribute("chg_ac")));
            msg->setChg_usb(convertToBool(value->getAttribute("chg_usb")));
            msg->setChg_wireless(convertToBool(value->getAttribute("chg_wireless")));
            msg->setIs_charging(convertToBool(value->getAttribute("is_charging")));
            eventList.push_back(eventPair(eventTimestamp, msg));
        } else if (strcmp(eventType, "CellularStatus") == 0) {
            //EV_INFO << "Handling " << eventType << "..." << endl;
            CellularEventMessage *msg = new  CellularEventMessage(eventType);
            msg->setCellular_type(value->getAttribute("cellular_type"));
            msg->setCellular_state(convertToInt(value->getAttribute("cellular_state")));
            eventList.push_back(eventPair(eventTimestamp, msg));
        } else if (strcmp(eventType, "AirplaneModeStatus") == 0) {
            //EV_INFO << "Handling " << eventType << "..." << endl;
            AirplaneModeEventMessage *msg = new AirplaneModeEventMessage();
            msg->setAirplaneModeOn(convertToBool(value->getAttribute("airplane_mode")));
            eventList.push_back(eventPair(eventTimestamp, msg));
        } else if (strcmp(eventType, "WiFiStatus") == 0) {
            //EV_INFO << "Handling " << eventType << "..." << endl;
            WiFiEventMessage *msg = new WiFiEventMessage(eventType);
            msg->setWifi_status(convertToInt(value->getAttribute("wifi_status")));
            eventList.push_back(eventPair(eventTimestamp, msg));
        } else if (strcmp(eventType, "BluetoothStatus") == 0) {
            //EV_INFO << "Handling " << eventType << "..." << endl;
            BluetoothEventMessage *msg = new BluetoothEventMessage(eventType);
            msg->setBluetooth_status(convertToInt(value->getAttribute("bluetooth_status")));
            eventList.push_back(eventPair(eventTimestamp, msg));
        } else {
            EV_ERROR << "Unhandled event type: " << eventType << endl;
            if (std::find(unhandledEvents.begin(), unhandledEvents.end(), std::string(eventType)) == unhandledEvents.end()){
                unhandledEvents.push_back(std::string(eventType));
            }
        }

        /*
        EV_INFO << "Type:" << value->getAttribute("type") << " Status:" << value->getAttribute("status") << " Value:" << value->getNodeValue() << endl;
        simtime_t eventTime = SimTime(strtod(value->getNodeValue(), NULL)*1000, SimTimeUnit::SIMTIME_MS);

        EV_INFO << "Next event: " << eventTime << endl;
        ScreenEventMessage *msg = new ScreenEventMessage("ScreenEvent");

        msg->setInjectionTime(eventTime);
        msg->setPayloadType(EVENT_TYPE_SCREEN);


        std::string statusString = value->getAttribute("status");

        EV_INFO << "STATUS: " << statusString << endl;

        if (statusString == "on") msg->setScreenOn(true);
        else if (statusString == "off") msg->setScreenOn(false);
        else throw cRuntimeError("Not an On Off value");

        scheduleAt(eventTime, msg);
        */
    }
    for (auto ue:unhandledEvents) {
        EV_ERROR << "Unhandled Event: " << ue << endl;
    }

    eventList.sort(sortEventList);

    for (std::list<eventPair>::iterator it = eventList.begin(); it != eventList.end(); ++it){
        //EV_INFO << "List item " << it->first << " " << it->second << endl;
        scheduleAt(it->first, it->second);
        //delete it->second;
    }

    EV_INFO << allEvents.size() << " events in file" << endl;
    EV_INFO << eventList.size() << " events in vector" << endl;

}

void PhoneEventInjector::handleMessage(cMessage *msg)
{
    EV_INFO << "MSG to out" << endl;
    for (int i = 0; i<gateSize("out"); i++ )
        send(msg, "out", i);
}

}; // namespace
