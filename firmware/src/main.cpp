#include "MIDIUSB.h"
#include <EEPROM.h>

int pedalPin = 10;

int RXLED = 17;  // The RX LED has a defined Arduino pin

int lastKnownPedalState = 0;
int channel = 1;

// First parameter is the event type (0x0B = control change).
// Second parameter is the event type, combined with the channel.
// Third parameter is the control number number (0-119).
// Fourth parameter is the control value (0-127).
void controlChange(byte channel, byte control, byte value) {
    midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
    MidiUSB.sendMIDI(event);
}

void setup() {
    pinMode(RXLED, OUTPUT);
    pinMode(pedalPin, INPUT);
    Serial.begin(115200);
    lastKnownPedalState = digitalRead(pedalPin);
    int val = EEPROM.read(0);
    if (val >= 0 && val <= 16) {
        channel = val;
    }
}

void loop() {
    while (digitalRead(pedalPin) == lastKnownPedalState) {
        midiEventPacket_t rx;
        rx = MidiUSB.read();
        if (rx.header != 0) {
            if (rx.byte2 == 1 && rx.byte3 >= 0 && rx.byte3 <= 16) {
                channel = rx.byte3 - 1;
                EEPROM.write(0, channel);
            }
        }
    }
    int pedal = digitalRead(pedalPin);
    lastKnownPedalState = pedal;
    controlChange(channel, 64, pedal * 100);
    MidiUSB.flush();
    digitalWrite(RXLED, !pedal);
}