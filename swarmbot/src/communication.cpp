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


/* -------------------------- PRIVATE -------------------------- */

void setupCommunication(void) {
    pinMode(RECEIVE_PIN,  INPUT);
    pinMode(TRANSMIT_PIN, OUTPUT);
    pinMode(CARRIER,      OUTPUT);
    
    // use interrupts on the input comm pin to manage state of communication subsystem
    attachInterrupt(COMM_INTERR, setReceiving, RISING);
    
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
    sendBits(hsMsg, MSG_LEN); // sending handshake so other bot knows message is coming
    sendBits(msg, len);
    digitalWrite(TRANSMIT_PIN, LOW); // ensure low afterwards
}

void receive(unsigned bits[], unsigned len) {
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
    receiving = false; // we are receiving, not waiting
    //if (receivingBot) transmit(ack, MSG_LEN);
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

void waiting(const unsigned msg[], const unsigned optionalMsg[] ) {
  do {
    do { transmit(optionalMsg, MSG_LEN); }
    while (!receiving);
    receive(recMsg, MSG_LEN);
  } while (!checkMsg(recMsg, msg, MSG_LEN));
}

void waiting(const unsigned msg[]) {
  do {
    do { delayMicroseconds(1); }
    while (!receiving);
    receive(recMsg, MSG_LEN);
  } while (!checkMsg(recMsg, msg, MSG_LEN));
}
