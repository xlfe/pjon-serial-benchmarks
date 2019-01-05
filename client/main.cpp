
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#define PJON_INCLUDE_ANY
#define PJON_INCLUDE_TSA
#define PJON_INCLUDE_TS

#define TSA_RESPONSE_TIME_OUT 50000
#include <PJON.h>

struct pps {
    double rate_;            // The average rate
    double last_;            // Accumulates bytes added until average is computed
    time_t prev_;            // Time of previous update
    pps () : rate_(0), last_(0), prev_(time(0)) {}
    void packet () {
        time_t now = time(0);
        if (now - prev_ < 1) {
            last_ += 1;
        } else {
            rate_ = (rate_ + last_)/2;
            printf("Rate: %f\n", rate_);
            last_ = 0;            // Reset last_ (it has been averaged in)
            prev_ = now;          // Update prev_ to current time
        }
    }
};

uint32_t baud_rate = 115200;
char serial_port[] = "/dev/tty.wchusbserial1410";

char *data = (char*)malloc(128);

PJON<ThroughSerial> bus(45);
pps avg;

void receiver_function(uint8_t *payload, uint16_t length, const PJON_Packet_Info &packet_info) {
    if(payload[0] == 'B') {
        avg.packet();
        bus.reply(data,32);
    }
}


void error_handler(uint8_t code, uint16_t data, void *custom_pointer) {
    if(code == PJON_CONNECTION_LOST) {
        printf("Connection lost\n");
    }
    if(code == PJON_PACKETS_BUFFER_FULL) {
        printf("Packet buffer is full, has now a length of %d\n",data);
    }
    if(code == PJON_CONTENT_TOO_LONG) {
        printf("Content is too long, length: %d\n",data);
    }
}


int main() {
    memset((char*) data, 0x42,128);

    int s = serialOpen(serial_port, baud_rate);

    if(s < 0) {
        printf("Serial open fail!");
        return 1;
    }

    bus.strategy.set_serial(s);
    bus.strategy.set_baud_rate(baud_rate);
    bus.set_receiver(receiver_function);
    bus.set_error(error_handler);
    bus.set_asynchronous_acknowledge(false);
    bus.set_synchronous_acknowledge(true);

    bus.begin();

    bus.send(44, "B", 1);

    while(true) {
        bus.update();
        bus.receive();
    }
};

