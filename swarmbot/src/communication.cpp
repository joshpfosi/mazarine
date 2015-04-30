/* communication.c
 *
 * This code will enable the bots to communication in order to perform together
 * as a swarmbot.
 *
 */

#include "Arduino.h"
#include "communication.h"
#include "pinmap.h"

#define CARRIER        5
#define DATA_DELAY     416  //  1/(1.2*10^3)/2 = 416 aka 1.2 kHz
#define BIT_SIZE       25
#define TIMEOUT        1000
#define READ_THRESHOLD 700

// received message buffer
static unsigned recMsg[] = { 
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

/* -------------------------- PRIVATE -------------------------- */

void setupCommunication(void) {
    pinMode(RECEIVE_PIN,  INPUT);
    pinMode(TRANSMIT_PIN, OUTPUT);
    pinMode(CARRIER,      OUTPUT);

    TCCR3A = _BV(COM3A0) | _BV(COM3B0) | _BV(WGM30) | _BV(WGM31);
    // sets COM Output Mode to FastPWM with toggle of OC3A on compare match with OCR3A
    // also sets WGM to mode 15: FastPWM with top set by OCR3A

    TCCR3B = _BV(WGM32) | _BV(WGM33) |  _BV(CS31);
    // sets WGM as stated above; sets clock scaling to "divide by 8"

    OCR3A = 55; // Used to be 63
    // above sets the counter value at which register resets to 0x0000;
    // generate 20kHz when OCR3A=50
}

/* -------------------------- INTERFACE -------------------------- */

void sendBits(const unsigned bits[], unsigned len) {
    for (unsigned i = 0; i < len; ++i) {
        for (unsigned j = 0; j < BIT_SIZE; ++j) {
            digitalWrite(TRANSMIT_PIN, (bits[i]) ? HIGH : LOW);
            delayMicroseconds(DATA_DELAY); 
        }    
    }
}

void transmit(const unsigned msg[], unsigned len) {
    sendBits(msg, len);
    digitalWrite(TRANSMIT_PIN, LOW); // ensure low afterwards
}

bool receive(unsigned bits[], unsigned len) {
    static unsigned long prevMillis = millis();
    while (analogRead(RECEIVE_PIN) <= READ_THRESHOLD) {
        if (millis() - prevMillis > TIMEOUT) {
            prevMillis = millis();
            Serial.println("receive timing out..."); // debug
            return false;
        }
    }

    for (unsigned i = 0; i < len; ++i) {
        unsigned sum = 0;
        for (unsigned j = 0; j < BIT_SIZE; ++j) {
            // read all 25 bits, recording sum
            sum += analogRead(RECEIVE_PIN) > READ_THRESHOLD;
            delayMicroseconds(DATA_DELAY);
        }

        // average sum of 25 bits, adding value to bit[i]
        bits[i] = ((float)sum / BIT_SIZE > 0.5);
    }

    Serial.println("received a message..."); // debug
    return true;
}

bool checkMsg(const unsigned msgToCheck[], const unsigned correctMsg[], unsigned len) {
    Serial.print("Checking message: ");
    for (int i = 0; i < len; ++i) Serial.print(msgToCheck[i]);
    Serial.println("\n");

    unsigned i, wrong = 0;
    for (i = 0; i < len; ++i) {
        if (msgToCheck[i] != correctMsg[i]) wrong++;
    }
    char buf[100]; sprintf(buf, "%d / %d = %f", wrong, len, (double)wrong / (double)len);
    Serial.println(buf);
    return ((float)wrong / (float)len) < 0.5;
}

// send a message and wait until acknowledgement
void sendAndWait(const unsigned msgToSend[], const unsigned confirmation[]) {
    Serial.println("Transmitting until ack...");

    while (1) {
        Serial.println("Transmitting msg");
        transmit(msgToSend, MSG_LEN);

        Serial.println("Waiting for ack...");
        if (receive(recMsg, MSG_LEN) && 
                checkMsg(recMsg, confirmation, MSG_LEN)) break;
        delay(750); // transmit only every 750 ms
    }
}

// waiting for a communication
void waiting(const unsigned msg[]) {
    while (1) {
        if (receive(recMsg, MSG_LEN) && checkMsg(recMsg, msg, MSG_LEN))
            break;
    }
    //while (!receive(recMsg, MSG_LEN) ||
    //        !checkMsg(recMsg, msg, MSG_LEN)) {};

    unsigned long prevMillis = millis();
    while (millis() - prevMillis < 10000) {
        Serial.println("Sending ack...");
        transmit(ack, MSG_LEN); // send acknowledgement only after msg received
        delay(100);
    }
}
