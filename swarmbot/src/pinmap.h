// pinmap.h
// Contains macro definitions to map pins for the final software design

#ifndef PINMAP_H
#define PINMAP_H

#include "Arduino.h"

// Interrupt pins

#define COLLISION_INT    0 // pin 2
#define COMM_INT         1 // pin 3

// Overall pins TODO new name

#define GO_SWITCH        14
#define BOT_SWITCH       15
#define RED_LED          1
#define YELLOW_LED       1
#define GREEN_LED        1
#define BLUE_LED         1

// Photosensor pins

#define PHOTO_RED_LEFT   9
#define PHOTO_RED_RIGHT  10
#define PHOTO_BLUE_LEFT  11
#define PHOTO_BLUE_RIGHT 12
                         
#define PHOTOLEFT        A0
#define PHOTORIGHT       A2 // should be A1 but wire stuck in it

// Motor Control pins

#define ML1              24
#define ML2              25
#define MR1              30
#define MR2              31

#define E1               4
#define E2               5

// Collision Detection pins

#define FL               43
#define FR               44
#define L                45
#define R                46
#define B                47

#endif // PINMAP_H
