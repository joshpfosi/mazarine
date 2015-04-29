/* communication.c
 *
 * This code will enable the bots to communication in order to perform together
 * as a swarmbot.
 *
 */

#include "Arduino.h"
#include "communication.h"
#include "pinmap.h"

#define CARRIER      5
#define DATA_DELAY   416  //  1/(1.2*10^3)/2 = 416 aka 1.2 kHz
#define BIT_SIZE     25
#define TIMEOUT      1000

const unsigned hsMsg[]    = { 1,1,1,1,0,0,0,0 };

// received message buffer
static unsigned recMsg[]    = { 0,0,0,0,0,0,0,0,
                                0,0,0,0,0,0,0,0 };

// communication flag
static bool receiving    = false;

/* -------------------------- PRIVATE -------------------------- */

void setupCommunication(void) {
    pinMode(RECEIVE_PIN,  INPUT);
    pinMode(TRANSMIT_PIN, OUTPUT);
    pinMode(CARRIER,      OUTPUT);

    // use interrupts on the input comm pin to manage state of communication subsystem
    attachInterrupt(COMM_INT, setReceiving, RISING);

    TCCR3A = _BV(COM3A0) | _BV(COM3B0) | _BV(WGM30) | _BV(WGM31);
    // sets COM Output Mode to FastPWM with toggle of OC3A on compare match with OCR3A
    // also sets WGM to mode 15: FastPWM with top set by OCR3A

    TCCR3B = _BV(WGM32) | _BV(WGM33) |  _BV(CS31);
    // sets WGM as stated above; sets clock scaling to "divide by 8"

    OCR3A = 55; // Used to be 63
    // above sets the counter value at which register resets to 0x0000;
    // generate 20kHz when OCR3A=50
}

void setReceiving(void) { receiving = true; }

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
    noInterrupts();

    sendBits(hsMsg, MSG_LEN); // sending handshake so other bot knows message is coming
    sendBits(msg, len);
    digitalWrite(TRANSMIT_PIN, LOW); // ensure low afterwards

    interrupts();
}

void receive(unsigned bits[], unsigned len) {
    noInterrupts();

    for (unsigned i = 0; i < 2 * len; ++i) {
        unsigned sum = 0;
        for (unsigned j = 0; j < BIT_SIZE; ++j) {
            // read all 25 bits, recording sum
            int x = digitalRead(RECEIVE_PIN);
            sum += x;
            delayMicroseconds(DATA_DELAY);
        }
        // average sum of 25 bits, adding value to bit[i]
        bits[i] = ((float)sum / BIT_SIZE > 0.5);
    }

    interrupts();
}

bool checkMsg(const unsigned msgToCheck[], const unsigned correctMsg[], unsigned len) {
    unsigned i;
    for (i = 0; i < len; ++i) {
        if (hsMsg[i] != msgToCheck[i] || msgToCheck[i+len] != correctMsg[i]) {
            return false;
        }
    }
    return true;
}

// send a message and wait until acknowledgement
void sendAndWait(const unsigned msgToSend[], const unsigned confirmation[]) {
    do {
        Serial.println("Transmitting until ack...");
        receiving = false;

        while (1) {
            unsigned long prevMillis = millis();

            transmit(msgToSend, MSG_LEN);

            // wait until receiving or timeout
            while (!receiving && millis() - prevMillis < 1000) {}

            if (receiving) break;
        }

        Serial.print("Receiving ack: ");
        receive(recMsg, MSG_LEN);
        for (int i = 0; i < 2 * MSG_LEN; ++i) Serial.print(recMsg[i]);
        Serial.println("\n");
    } while (!checkMsg(recMsg, confirmation, MSG_LEN));
}

// waiting for a communication
void waiting(const unsigned msg[]) {
    //digitalWrite(TRANSMIT_PIN, LOW); // needed for an unknown reason
    do {
        receiving = false;
        while (!receiving) { delayMicroseconds(1); }

        receive(recMsg, MSG_LEN);
        Serial.print("Transmitted message: ");
        for (int i = 0; i < 2 * MSG_LEN; ++i) Serial.print(recMsg[i]);
        Serial.println("\n");
    } while (!checkMsg(recMsg, msg, MSG_LEN));
}
