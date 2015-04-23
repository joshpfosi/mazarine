#include "pinmap.h"
#include "motor_control.h"
#include "photo_sensor.h"
#include "communication.h"

// ----------------------------------------------------------------------------
// State variables
static bool isBot1  = true;
static bool hitWall = false;
// ----------------------------------------------------------------------------

/* Communication messages */
static const unsigned transMsg[] = { 1,1,0,0,1,1,1,0 };
static const unsigned startMsg[] = { 1,0,1,0,1,0,1,0 };
static const unsigned toxicMsg[] = { 1,1,0,0,1,1,0,0 };
static const unsigned stopMsg[]  = { 0,0,1,1,0,0,1,1 };
static const unsigned doneMsg[]  = { 1,1,1,1,1,1,1,1 };
static unsigned recMsg[]         = { 0,0,0,0,0,0,0,0,
                                     0,0,0,0,0,0,0,0 };
                        
void bot1(void);
void bot2(void);

// Helper routines
static inline void flashLed(int ledPin);

// ----------------------------------------------------------------------------
// collision.h
#define NUM_BUMPERS 5
#define DEBOUNCE_TIME 10000 // ms

// indices into boolean array for each bumper
// and bumper array for each pin
#define FRONT_LEFT  0
#define FRONT_RIGHT 1
#define LEFT        2
#define RIGHT       3
#define BACK        4

// input pins for reading if a bumper is pressed

bool bumperHit[NUM_BUMPERS];
volatile bool collisionHappened = false;
const int bumpers[] = { FL, FR, L, R, B };
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// collision.cpp
void detectCollision(void) {
    static unsigned long lastInterruptTime;
    unsigned long currTime = micros();

    if (currTime - lastInterruptTime > DEBOUNCE_TIME) {
        collisionHappened = true;
        lastInterruptTime = currTime;
    }
}

void setupCollision(void) {
    attachInterrupt(COLLISION_INT, detectCollision, RISING);
    for (int i = 0; i < NUM_BUMPERS; ++i) pinMode(bumpers[i], INPUT);
}
// ----------------------------------------------------------------------------

void setup() {
    // ------------------------------------------------------------------------
    // Set up general Bot pins
    //pinMode( GO_SWITCH,  INPUT);
    //pinMode( BOT_SWITCH, INPUT);
    pinMode( RED_LED,    OUTPUT);
    pinMode( YELLOW_LED, OUTPUT);
    // ------------------------------------------------------------------------

    Serial.begin(9600);

    setupCommunication();
    setupPhotosensor();
    setupMotorControl();
    setupCollision();
}

void loop() {
    // TEST COLLISION
    //if (collisionHappened) {
    //    for (int i = 0; i < NUM_BUMPERS; ++i) {
    //        bumperHit[i] = digitalRead(bumpers[i]);
    //
    //        if (bumperHit[i]) {
    //            Serial.println(i);
    //            bumperHit[i] = false;
    //        }
    //    }
    //}

    // TEST MOTOR CONTROL
    //forward();
    //delay(1000);
    //backward();
    //delay(2000);
    //stop();
    //delay(500);

    // TEST PHOTOSENSOR
    //while (1) testPhotosensor();

    //while (!digitalRead(GO_SWITCH)) { delayMicroseconds(1); } // ON

    isBot1 = false; //digitalRead(BOT_SWITCH);

    // GO state
    if (isBot1) bot1();
    else        bot2();

    while (1) {};
}

void bot1(void) {
    // Puts bot in motion (`forward()`)
    forward();

    // Loops until collision (boolean is set in ISR)
    while (!hitWall) {
        for (int i = 0; i < NUM_BUMPERS; ++i) {
            if (digitalRead(bumpers[i])) {
                hitWall = true;
            }
        }
        delay(1);
    }
    Serial.println("Hit wall");


    // Assumes it hit the correct wall and turns a predetermined angle to the left
    stop(); delay(200); // resolve collision by backing up
    backward();
    delay(100);
    turn(-210); // negative to turn right

    // Moves forward until red and stops
    actionUntilColor(RED, forward);

    // Flash a red LED
    flashLed(RED_LED); // pauses for 1 second to flash

    // Follows red
    while (!followColorUntilColor(RED, YELLOW)) { delayMicroseconds(1); }
    Serial.println("Found yellow");

    actionUntilColor(YELLOW, turnLeft);
    // Stops on yellow, flashing yellow LED twice
    stop();
    flashLed(YELLOW_LED); // pauses for 1 second to flash
    flashLed(YELLOW_LED); // pauses for 1 second to flash

    // Moves forward until red
    actionUntilColor(RED, forward);

    // Follows red
    while (!followColorUntilColor(RED, YELLOW)) { delayMicroseconds(1); }

    // Stops on yellow, turns on yellow LED, turns 180 degrees
    stop();
    digitalWrite(YELLOW_LED, HIGH);
    turn(180);
    actionUntilColor(RED, turnLeft);

    // Moves forward until red
    actionUntilColor(RED, forward);

    // Follows red
    while (!followColorUntilColor(RED, YELLOW)) { delayMicroseconds(1); }

    // Stops on yellow
    stop();

    // Communicates to Bot 2: `START`

    // Waits for `ACK_START`  
    do {
      transmit(startMsg, MSG_LEN);
      receive(recMsg, MSG_LEN);
    }
    while (!checkMsg(recMsg, startMsg, MSG_LEN));
        
    // Flashes green LED
    flashLed(GREEN_LED);

    // Moves forward until red
    actionUntilColor(RED, forward);

    // Follows red, stops on yellow
    while (!followColorUntilColor(RED, YELLOW)) { delayMicroseconds(1); }

    // Turns on green LED
    flashLed(GREEN_LED);

    // Waits for `TOXIC`
    delay(100);

    // Flash yellow LED continuously
    // TODO flash LED continously

    // Waits for `STOP_YELLOW`
    delay(100);

    // Turns off yellow LED
    digitalWrite(YELLOW_LED, LOW);

    // Waits for `DONE`
    delay(100);

    // Flashes green LED
    flashLed(GREEN_LED);
}


void bot2(void) {
    // Waits for `START`
    delay(500);

    // Flash green LED
    flashLed(GREEN_LED);

    // Puts bot in motion (`forward()`)
    forward();

    // Loops until collision (boolean is set in ISR)
    while (!hitWall) {
        for (int i = 0; i < NUM_BUMPERS; ++i) {
            if (digitalRead(bumpers[i])) {
                hitWall = true;
            }
        }
        delay(1);
    }
    Serial.println("Hit wall");

    // Assumes it hit the correct wall and turns a predetermined angle to the left
    stop(); delay(200); // resolve collision by backing up
    backward();
    delay(100);
    turn(300); // negative to turn left CALIBRATED
    
    // Moves forward 
    actionUntilColor(BLUE, forward);

    // Stops on blue, flashing a blue LED
    flashLed(BLUE_LED);

    // Turns a predetermined angle to the right, and follows blue
    while (!followColorUntilColor(BLUE, YELLOW)) { delayMicroseconds(1); }
    Serial.println("Found yellow");

    // Stops on yellow, flashing yellow LED twice
    actionUntilColor(YELLOW, turnLeft);

    flashLed(YELLOW_LED);
    flashLed(YELLOW_LED);

    // Moves forward
    actionUntilColor(BLUE, forward);

    // Follows blue
    while (!followColorUntilColor(BLUE, YELLOW)) { delayMicroseconds(1); }

    // Stops on yellow, turns on yellow LED, turns 180 degrees
    stop();
    digitalWrite(YELLOW_LED, HIGH);
    turn(-180);
    actionUntilColor(BLUE, turnRight);

    // Communicates to Bot 1: `TOXIC`
    delay(100);
    // Flash yellow continuously
    // TODO flash continuously

    // Moves forward until blue
    actionUntilColor(BLUE, forward);

    // Follows blue, stops on yellow
    while (!followColorUntilColor(BLUE, YELLOW)) { delayMicroseconds(1); }

    // Communicates to Bot 1: `STOP_YELLOW`
    delay(100);

    // Stop flashing yellow LED
    digitalWrite(YELLOW_LED, LOW);

    // Moves forward until blue
    actionUntilColor(BLUE, forward);

    // Follows blue, stops on yellow
    while (!followColorUntilColor(BLUE, YELLOW)) { delayMicroseconds(1); }

    // Communicates to Bot 1: `DONE`
    delay(100);

    // Flashes green LED
    flashLed(GREEN_LED);
}

// ----------------------------------------------------------------------------
// Helper Routines
// ----------------------------------------------------------------------------

static inline void flashLed(int ledPin) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
}
