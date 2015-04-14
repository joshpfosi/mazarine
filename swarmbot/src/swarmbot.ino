#include "pinmap.h"
#include "photo_sensor.h"
#include "collision.h"
#include "motor_control.h"

// ----------------------------------------------------------------------------
// State variables
static bool isBot1  = true;
static bool hitWall = false;
// ----------------------------------------------------------------------------


//#define NUM_BUMPERS 5
//#define DEBOUNCE_TIME 10000 // ms
//
//// indices into boolean array for each bumper
/// and bumper array for each pin
//#define FRONT_LEFT  0
//#define FRONT_RIGHT 1
//#define LEFT        2
//#define RIGHT       3
//#define BACK        4
//
//// input pins for reading if a bumper is pressed
//
//bool bumperHit[NUM_BUMPERS];
//volatile bool collisionHappened = false;
//const int bumpers[] = { FL, FR, L, R, B };
//
//void setupCollision(void) {
//    for (int i = 0; i < NUM_BUMPERS; ++i) pinMode(bumpers[i], INPUT);
//}
//
void setup() {
    // ------------------------------------------------------------------------
    // Set up general Bot pins
    pinMode( GO_SWITCH,  INPUT);
    pinMode( BOT_SWITCH, INPUT);
    // ------------------------------------------------------------------------

    Serial.begin(9600);

    //setupPhotosensor();
    pinMode(PHOTO_RED_LEFT,   OUTPUT);
    pinMode(PHOTO_RED_RIGHT,  OUTPUT);
    pinMode(PHOTO_BLUE_LEFT,  OUTPUT);
    pinMode(PHOTO_BLUE_RIGHT, OUTPUT);
    pinMode(PHOTOLEFT,        INPUT);
    pinMode(PHOTORIGHT,       INPUT);
    //setupCollision();
    //
    attachInterrupt(0, detectCollision, CHANGE);
}
//
//void detectCollision(void) {
//    Serial.println("in loop");
//    static unsigned long lastInterruptTime;
//    unsigned long currTime = micros();
//
//    if (currTime - lastInterruptTime > DEBOUNCE_TIME) {
//        collisionHappened = true;
//        lastInterruptTime = currTime;
//    }
//}

void loop() {
    //test();
    if (collisionHappened) {
        Serial.println("collision Happened");
        for (int i = 0; i < NUM_BUMPERS; ++i) {
            bumperHit[i] = digitalRead(bumpers[i]);
    
            if (bumperHit[i]) {
                Serial.println(i);
                bumperHit[i] = false;
            }
        }
    }
    //while (!digitalRead(GO_SWITCH)) { delayMicroseconds(1); } // ON

    //isBot1 = digitalRead(BOT_SWITCH);

    //// GO state
    //if (isBot1) bot1();
    //else        bot2();
}

//void bot1(void) {
//    // Puts bot in motion (`forward()`)
//    forward();
//
//    // Loops until collision (boolean is set in ISR)
//    while (!hitWall) {
//        for (int i = 0; i < NUM_BUMPERS; ++i) hitWall = bumperHit[i];
//        delayMicroseconds(100); // just so we don't overload the processor
//                                // could be longer potentially
//    }
//
//    // Assumes it hit the correct wall and turns a predetermined angle to the left
//
//    backward(); delay(1); // resolve collision by backing up
//    turn(-20); // negative to turn left
//
//    // Moves forward 
//    forward();
//
//    // Stops on red, flashing a red LED
//    // Turns a predetermined angle to the right, and follows red
//    // Stops on yellow, flashing yellow LED twice
//    // Moves forward
//    // Follows red
//    // Stops on yellow, turns on yellow LED, turns 180 degrees
//    // Moves forward until red
//    // Follows red
//    // Stops on yellow
//    // Communicates to Bot 2: `START`
//    // Waits for `ACK_START`
//    // Flashes green LED
//    // Moves forward until red
//    // Follows red
//    // TBD // we want this to be close to a specific location
//    // Turns on green LED
//    // Waits for `TOXIC`
//    // Flash yellow LED continuously
//    // Waits for `STOP_YELLOW`
//    // Turns off yellow LED
//    // Waits for `DONE`
//    // Flashes green LED
//}
//
//void bot2(void) {
//    // Waits for `START`
//    // Flash green LED
//    // Puts bot in motion (`forward()`)
//    // Loops until collision (boolean is set in ISR)
//    // Assumes it hit the correct wall and turns a predetermined angle to the left
//    // Moves forward 
//    // Stops on blue, flashing a blue LED
//    // Turns a predetermined angle to the right, and follows blue
//    // Stops on yellow, flashing yellow LED twice
//    // Moves forward
//    // Follows blue
//    // Stops on yellow, turns on yellow LED, turns 180 degrees
//    // Communicates to Bot 1: `TOXIC`
//    // Flash yellow continuously
//    // Moves forward until blue
//    // Follows blue
//    // Stops on yellow
//    // Communicates to Bot 1: `STOP_YELLOW`
//    // Stop flashing yellow LED
//    // Moves forward until blue
//    // Follows blue
//    // TBD // we want this to be close to a specific location
//    // Communicates to Bot 1: `DONE`
//    // Flashes green LED
//}
