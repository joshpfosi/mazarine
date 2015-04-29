// pinmap.h
// Contains macro definitions to map pins for the final software design

#ifndef PINMAP_H
#define PINMAP_H

#include "Arduino.h"

#define MAZ         0
#define CALIBRATION 0
#define TEST_MOTOR  0
#define TEST_PHOTO  0
#define TEST_COMM   1

// Interrupt pins

#define COLLISION_INT    1 // pin 3
#define COMM_INT         0 // pin 2

// Overall pins

#define CALIB_SWITCH     14
#define BOT_SWITCH       15
#define RED_LED          32
#define YELLOW_LED       33
#define GREEN_LED        34
#define BLUE_LED         35

// Photosensor pins

#define PHOTO_RED_LEFT   9
#define PHOTO_RED_RIGHT  8
#define PHOTO_BLUE_LEFT  11
#define PHOTO_BLUE_RIGHT 12
                         
#define PHOTOLEFT        A0
#define PHOTORIGHT       A1

// Motor Control pins

#define ML1              24
#define ML2              25
#define MR1              30
#define MR2              31

#define EL               4
#define ER               5

// Collision Detection pins

#define FL               43
#define FR               44
#define L                45
#define R                46
#define B                47

// Communication pins

#define RECEIVE_PIN      4
#define TRANSMIT_PIN     12

#endif // PINMAP_H
