#include "pinmap.h"
#include "motor_control.h"

// ----------------------------------------------------------------------------
// State variables
static bool isBot1  = true;
static bool hitWall = false;
// ----------------------------------------------------------------------------

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
// photo_sensor.h
enum Colors { RED, BLUE, YELLOW, BLACK };

inline void readRightSensor (int *r, int *b);
inline void readLeftSensor  (int  *r, int *b);

inline bool isBlue  (int red, int blue);
inline bool isRed   (int red, int blue);
inline bool isYellow(int red, int blue);

inline bool followColorUntilColor(Colors c1, Colors c2);

void setupPhotosensor(void);
void testPhotosensor(void);
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

// ----------------------------------------------------------------------------
// photosensor.cpp
void setupPhotosensor(void) {
    pinMode(PHOTO_RED_LEFT,   OUTPUT);
    pinMode(PHOTO_RED_RIGHT,  OUTPUT);
    pinMode(PHOTO_BLUE_LEFT,  OUTPUT);
    pinMode(PHOTO_BLUE_RIGHT, OUTPUT);
    pinMode(PHOTOLEFT,        INPUT);
    pinMode(PHOTORIGHT,       INPUT);
}

inline bool isBlue(int red, int blue, bool left) { 
    if (left) return (900 < red && red < 1024) && (500 < blue && blue < 650);
    else      return (900 < red && red < 1024) && (150 < blue && blue < 250);
}

inline bool isRed(int red, int blue, bool left) { 
    if (left) return (700 < red && red < 800) && (900 < blue && blue < 1024);
    else      return (800 < red && red < 900) && (650 < blue && blue < 750);
}

inline bool isYellow(int red, int blue, bool left) { 
    if (left) return (600 < red && red < 700) && (750 < blue && blue < 850);
    else      return (700 < red && red < 800) && (150 < blue && blue < 250);
}

//
// Read sensor value for red, blue and both
//
inline Colors readLeftSensor(void) {
    int r, b;

    digitalWrite(PHOTO_BLUE_LEFT, LOW);
    digitalWrite(PHOTO_RED_LEFT,  HIGH);
    delay(30);
    r = analogRead(PHOTOLEFT);
    
    digitalWrite(PHOTO_RED_LEFT,  LOW);
    digitalWrite(PHOTO_BLUE_LEFT, HIGH);
    delay(30);
    b = analogRead(PHOTOLEFT);

    digitalWrite(PHOTO_BLUE_LEFT, LOW);

    //char buf[100]; sprintf(buf, "red=%d, blue=%d", r,b);
    //Serial.println(buf);
    Colors color;
    if (isBlue(r,        b, true)) color = BLUE;
    else if (isRed(r,    b, true)) color = RED;
    else if (isYellow(r, b, true)) color = YELLOW;
    else                       color = BLACK;

    //switch (color) {
    //    case RED:    Serial.println("Left is red");    break;
    //    case BLUE:   Serial.println("Left is blue");   break;
    //    case YELLOW: Serial.println("Left is yellow"); break;
    //    case BLACK:  Serial.println("Left is black");  break;
    //    default:     Serial.println("We're fucked if we're reading this!");
    //}
    
    return color;
}

inline Colors readRightSensor(void) {
    int r, b;
    
    digitalWrite(PHOTO_BLUE_RIGHT, LOW);
    digitalWrite(PHOTO_RED_RIGHT,  HIGH);
    delay(30);
    r = analogRead(PHOTORIGHT);
    
    digitalWrite(PHOTO_RED_RIGHT,  LOW);
    digitalWrite(PHOTO_BLUE_RIGHT, HIGH);
    delay(30);
    b = analogRead(PHOTORIGHT);

    digitalWrite(PHOTO_BLUE_RIGHT, LOW);

    Colors color;
    //char buf[100]; sprintf(buf, "red=%d, blue=%d", r,b);
    //Serial.println(buf);
    if (isBlue(r,   b, false)) color = BLUE;
    else if (isRed(r,    b, false)) color = RED;
    else if (isYellow(r, b, false)) color = YELLOW;
    else                       color = BLACK;

    //switch (color) {
    //    case RED:    Serial.println("Right is red");    break;
    //    case BLUE:   Serial.println("Right is blue");   break;
    //    case YELLOW: Serial.println("Right is yellow"); break;
    //    case BLACK:  Serial.println("Right is black");  break;
    //    default:     Serial.println("We're fucked if we're reading this!");
    //}
    
    return color;
}

//
// Test calibration by serial printing colors
//
void testPhotosensor(void) {
    Colors colorLeft = readLeftSensor();
    Colors colorRight = readRightSensor();

    delay(100);
    // NOTE assumes Serial is setup via Serial.begin(9600)
    switch (colorLeft) {
        case RED:    Serial.println("Left is red");    break;
        case BLUE:   Serial.println("Left is blue");   break;
        case YELLOW: Serial.println("Left is yellow"); break;
        case BLACK:  Serial.println("Left is black");  break;
        default:     Serial.println("We're fucked if we're reading this!");
    }

    switch (colorRight) {
        case RED:    Serial.println("Right is red");    break;
        case BLUE:   Serial.println("Right is blue");   break;
        case YELLOW: Serial.println("Right is yellow"); break;
        case BLACK:  Serial.println("Right is black");  break;
        default:     Serial.println("We're fucked if we're reading this!");
    }
}

inline void forwardUntilColor(Colors c) {
    while (readLeftSensor() != c && readRightSensor() != c) forward();
    stop();
}

inline bool followColorUntilColor(Colors c1, Colors c2) {
    Colors colorLeft  = readLeftSensor();
    Colors colorRight = readRightSensor();

    if (colorLeft == c2 || colorRight == c2)        {
        stop(); 
        return true;
    }
    else if (colorLeft == c1 && colorRight != c1) {
        //Serial.println("Right is off");
        turnLeft();
        do {
            colorRight = readRightSensor();
        } while (colorRight != c1 && colorRight != c2);
        stop();
          
    }
    else if (colorLeft != c1 && colorRight == c1) {
        //Serial.println("Left is off");
        turnRight();
        do {
            colorLeft  = readLeftSensor();
        } while (colorLeft != c1 && colorLeft != c2);
        stop();
    }
    else if ((colorLeft == c1 && colorRight == c1)) {
        //Serial.println("Both on color ");
        forward();
        return false;
    }

    return false;
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

    isBot1 = true; //digitalRead(BOT_SWITCH);

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
    turn(-180); // negative to turn right

    // Moves forward until red and stops
    forwardUntilColor(RED);

    // Flash a red LED
    flashLed(RED_LED); // pauses for 1 second to flash

    // Follows red
    while (!followColorUntilColor(RED, YELLOW)) { delayMicroseconds(1); }
    Serial.println("Found yellow");

    // Stops on yellow, flashing yellow LED twice
    stop();
    flashLed(YELLOW_LED); // pauses for 1 second to flash
    flashLed(YELLOW_LED); // pauses for 1 second to flash

    // Moves forward until red
    forwardUntilColor(RED);

    // Follows red
    while (!followColorUntilColor(RED, YELLOW)) { delayMicroseconds(1); }

    // Stops on yellow, turns on yellow LED, turns 180 degrees
    stop();
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
}

void bot2(void) {
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
}

// ----------------------------------------------------------------------------
// Helper Routines
// ----------------------------------------------------------------------------

static inline void flashLed(int ledPin) {
    digitalWrite(ledPin, HIGH);
    delay(500);
    digitalWrite(ledPin, LOW);
    delay(500);
}
