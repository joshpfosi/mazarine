// pinmap.h
// Contains macro definitions to map pins for the final software design

#ifndef PINMAP_H
#define PINMAP_H

#include "Arduino.h"

// Interrupt pins

#define COLLISION_INT    0 // pin 2
#define COMM_INT         1 // pin 3

// Overall pins TODO new name

#define GO_SWITCH        1
#define BOT_SWITCH       1
#define RED_LED          1
#define YELLOW_LED       1
#define GREEN_LED        1
#define BLUE_LED         1

// Photosensor pins

#define PHOTO_RED_LEFT   2
#define PHOTO_RED_RIGHT  3
#define PHOTO_BLUE_LEFT  4
#define PHOTO_BLUE_RIGHT 5

#define PHOTOLEFT        A0
#define PHOTORIGHT       A1

// Motor Control pins

#define ML1              30
#define ML2              31
#define MR1              32
#define MR2              33

#define E1               A2
#define E2               A3

// Collision Detection pins

#define FL               43
#define FR               44
#define L                45
#define R                46
#define B                47

#endif // PINMAP_H
