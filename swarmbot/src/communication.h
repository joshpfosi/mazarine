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

void transmit(const unsigned msg[], unsigned len, unsigned timeout);
bool receive(unsigned bits[], unsigned len);

#endif // COMMUNICATION_H
