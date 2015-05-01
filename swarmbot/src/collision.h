#ifndef COLLISION_H
#define COLLISION_H

#include "pinmap.h"

#define NUM_BUMPERS 5

static volatile bool bumperHit[NUM_BUMPERS];
static volatile bool collisionHappened;
const int bumpers[] = { FL, FR, L, R, B };

void detectCollision(void);
void setupCollision(void);

#endif // COLLISION_H
