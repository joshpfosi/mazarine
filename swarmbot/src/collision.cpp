//#include "pinmap.h"
//#include "collision.h"
//
//#define DEBOUNCE_TIME 10000 // ms
//
//void setupCollision(void) {
//    //attachInterrupt(COLLISION_INT, detectCollision, RISING);
//    for (int i = 0; i < NUM_BUMPERS; ++i) pinMode(bumpers[i], INPUT);
//}
//
//void detectCollision(void) {
//    static unsigned long lastInterruptTime;
//    unsigned long currTime = micros();
//
//    if (currTime - lastInterruptTime > DEBOUNCE_TIME) {
//        collisionHappened = !collisionHappened;
//        lastInterruptTime = currTime;
//    }
//}
