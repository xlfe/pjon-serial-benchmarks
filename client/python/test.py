import random, sys
import pjon_cython as PJON


class ThroughSerial(PJON.ThroughSerialAsync):
    def receive(self, data, length, packet_info):
        if data.startswith(b'B'):
            print('.'),
            # self.count += 1
            if random.random() > 0.8:
                self.reply(b'B')

ts = ThroughSerial(45, b"/dev/tty.wchusbserial1410", 115200)
ts.set_asynchronous_acknowledge(False)
ts.set_synchronous_acknowledge(True)
ts.count = 0

ts.send(44, b'B')

while True:
    try:
        ts.loop()
    except:
        print("Unexpected error:", sys.exc_info()[0])


