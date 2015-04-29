/* communication.h
 * This file exposes the communication interface
 */

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include "Arduino.h"

#define MSG_LEN 8

/* ISR */
void setReceiving(void);

/* sets up pins for communication */
void setupCommunication(void);

/* communication sending wrapper function */
void transmit(const unsigned msg[], unsigned len);

/* send bits of length len */
void sendBits(const unsigned bits[], unsigned len);

/* receive bits of length len */
void receive(unsigned bits[], unsigned len);

/* check sent message is received message */
bool checkMsg(const unsigned msgToCheck[], const unsigned correctMsg[], unsigned len);

/* sending and waiting until specific message is received */
void waiting(const unsigned msg[], const unsigned optionalMsg[] );  // overloaded with transmitting message
void waiting(const unsigned msg[]);

/* received message buffer */
static const unsigned ack[]      = { 0,0,0,0,1,1,1,1 };
static unsigned recMsg[]         = { 0,0,0,0,0,0,0,0,
                                     0,0,0,0,0,0,0,0 };

/* communication flag */
static bool receiving    = false;

#endif // COMMUNICATION_H
