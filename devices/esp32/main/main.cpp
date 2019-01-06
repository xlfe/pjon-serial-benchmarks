#include "Arduino.h"

#define PJON_INCLUDE_ANY
#define PJON_INCLUDE_TSA
#define PJON_INCLUDE_TS

#define TSA_RESPONSE_TIME_OUT 50000
#include "PJON.h"


PJON<Any> *bus = NULL;

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
    if(payload[0] == 'B') {
        bus->reply("B", 1);
    }
}

void loop() {
    bus->update();
    bus->receive();
}

void setup() {

    Serial.begin(115200);

    //Serial
    StrategyLink <ThroughSerialAsync> *link_tsa = new StrategyLink<ThroughSerialAsync>;
    link_tsa->strategy.set_serial(&Serial);

    bus = new PJON<Any>(44);
    bus->strategy.set_link(link_tsa);
    bus->set_receiver(receiver_function);
    bus->set_asynchronous_acknowledge(false);
    bus->set_synchronous_acknowledge(true);

//    bus->send_repeatedly(45, "B", 1, 10000);


}
