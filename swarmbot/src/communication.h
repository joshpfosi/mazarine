/* communication.h
 * This file exposes the communication interface
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "Arduino.h"
#include "pinmap.h"

#define MSG_LEN 128

// ISR
void setReceiving(void);

// sets up pins for communication
void setupCommunication(void);

// communication sending wrapper function
void transmit(const unsigned msg[], unsigned len);

// send bits of length len
void sendBits(const unsigned bits[], unsigned len);

// receive bits of length len
bool receive(unsigned bits[], unsigned len);

// check sent message is received message
bool checkMsg(const unsigned msgToCheck[], const unsigned correctMsg[], unsigned len);

// sending and waiting until specific message is received
void sendAndWait(const unsigned msgToSend[], const unsigned confirmation[]);
void waiting(const unsigned msg[]);

#endif // COMMUNICATION_H
