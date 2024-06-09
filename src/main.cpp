#include <Arduino.h>

#define CS PA2
#define CLK PA3
#define DIO PA4
#define RELAYLED PC13
#define RELAYOUT PA1

void setup(void)
{
    pinMode(CS, OUTPUT);
    pinMode(CLK, OUTPUT);
    pinMode(DIO, INPUT);
    pinMode(RELAYLED, OUTPUT);
    pinMode(RELAYOUT, OUTPUT);
    Serial.begin(9600);
    digitalWrite(CS, 1);
    digitalWrite(RELAYOUT, 0);
    digitalWrite(RELAYLED, 1);
}

uint32_t pollSerial(void)
{
    uint32_t data = 0;
    digitalWrite(CS, 0);
    digitalWrite(CLK, 0);
    delayMicroseconds(1000);
    for (int i = 0; i < 32; ++i) {
        digitalWrite(CLK, 1);
        delayMicroseconds(1000);
        data = data << 1;
        data |= digitalRead(DIO);
        digitalWrite(CLK, 0);
        delayMicroseconds(1000);
    }
    delayMicroseconds(1000);
    digitalWrite(CS, 1);
    return data;
}

int rawMode = 0;

void loop(void)
{
    if (!rawMode) {
        uint32_t data = pollSerial();
        if (data & 0x10000) {
            if (data & 1)
                Serial.println("ERROR OC");
            else if (data & 2)
                Serial.println("ERROR GND");
            else if (data & 4)
                Serial.println("ERROR VCC");
        } else {
            Serial.print(((int)data) >> 20, DEC);
            switch((data >> 18) & 3) {
                case 0: Serial.println(".00"); break;
                case 1: Serial.println(".25"); break;
                case 2: Serial.println(".50"); break;
                case 3: Serial.println(".75"); break;
            }
        }
    }
    for (int i = 0; i < 20; ++i) {
        if (rawMode)
            delayMicroseconds(10000);
        else
            delayMicroseconds(50000);
        while (Serial.available()) {
            int ch = Serial.read();
            switch(ch) {
            case '0':
                digitalWrite(RELAYOUT, 0);
                digitalWrite(RELAYLED, 1);
                break;
            case '1':
                digitalWrite(RELAYOUT, 1);
                digitalWrite(RELAYLED, 0);
                break;
            case 'r':
                rawMode = true;
                break;
            case 'c':
                rawMode = false;
                break;
            case '?':
                if (rawMode)
                {
                    uint32_t data = pollSerial();
                    Serial.println(data, HEX);
                }
                break;
            }
        }
    }
}
