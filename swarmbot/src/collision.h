#ifndef COLLISION_H
#define COLLISION_H

#include "Arduino.h"

#define NUM_BUMPERS 5

// indices into boolean array for each bumper
// and bumper array for each pin
#define FRONT_LEFT  0
#define FRONT_RIGHT 1
#define LEFT        2
#define RIGHT       3
#define BACK        4

// input pins for reading if a bumper is pressed

bool bumperHit[NUM_BUMPERS];
bool collisionHappened;
const int bumpers[] = { FL, FR, L, R, B };

#endif // COLLISION_H
