/* communication.h
 * This file exposes the communication interface
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "Arduino.h"
#include "pinmap.h"

#define MSG_LEN 128
// sets up pins for communication
void setupCommunication(void);

void transmit(const unsigned msg[], unsigned len);
bool receive(unsigned bits[], unsigned len);

// sending and waiting until specific message is received
void sendAndWait(const unsigned msgToSend[], const unsigned confirmation[]);
void waiting(const unsigned msg[]);

#endif // COMMUNICATION_H
