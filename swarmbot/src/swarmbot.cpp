#include "pinmap.h"
#include "motor_control.h"
#include "photo_sensor.h"
#include "collision.h"
#include "communication.h"

// ----------------------------------------------------------------------------
// State variables
static bool isBot1  = true;
static bool hitWall = false;
// ----------------------------------------------------------------------------

void bot1(void);
void bot2(void);

// Helper routines
inline void flashLed              (int ledPin);
inline void actionUntilColor      (Colors c, void (*action)(void));
inline bool followColorUntilColor (Colors c1, Colors c2);

/* Communication messages */
static const unsigned transMsg[] = { 1,1,0,0,1,1,0,0 };
static const unsigned startMsg[] = { 1,0,1,0,1,0,1,0 };
static const unsigned toxicMsg[] = { 1,1,0,0,1,1,0,0 };
static const unsigned stopMsg[]  = { 0,0,1,1,0,0,1,1 };
static const unsigned doneMsg[]  = { 1,1,1,1,1,1,1,1 };
static const unsigned ack[]      = { 0,0,0,0,1,1,1,1 };

void setup() {
    // ------------------------------------------------------------------------
    // Set up general bot pins
    // ------------------------------------------------------------------------

    pinMode(CALIB_SWITCH, INPUT);
    pinMode(BOT_SWITCH,   INPUT);

    pinMode(RED_LED,      OUTPUT);
    pinMode(YELLOW_LED,   OUTPUT);
    pinMode(GREEN_LED,    OUTPUT);
    pinMode(BLUE_LED,     OUTPUT);

    Serial.begin(9600);

    setupPhotosensor();
    setupMotorControl();
    setupCollision();
    setupCommunication();
}

void loop() {
#if TEST_MOTOR
    while (1) {
        forward();
        delay(1000);
        backward();
        delay(2000);
        stop();
        delay(500);
    }
#endif

#if TEST_PHOTO
    while (1) testPhotosensor();
#endif

#if TEST_COMM

    // TODO this should read isBot1 and decide
#if MAZ
    while(1) {
        sendAndWait(transMsg, ack);
        delay(2000);
    }
#else
    while (1) {
        Serial.println("Waiting for transMsg...");
        waiting(transMsg);

        Serial.println("Sending ack...");
        transmit(ack, MSG_LEN); // send acknowledgement only after msg received

        delay(2000);
    }
#endif

#endif
    
#if CALIBRATION
    bool hitBumper = false;
    int avgs[3][2] = { {0,0}, {0,0}, {0,0} };
    int avgNum = 64;
    for (int i = 0; i < 3; ++i) {
        unsigned sumLeft = 0, sumRight = 0, count = 0;

        while (!hitBumper) {
            for (int i = 0; i < NUM_BUMPERS; ++i) {
                if (digitalRead(bumpers[i])) {
                    hitBumper = true;
                    delay(1000);
                }
            }
        }

        for (int j = 0; j < avgNum; ++j) {
            sumLeft  += analogRead(PHOTOLEFT);
            sumRight += analogRead(PHOTORIGHT);
            delay(1);
        }


        avgs[i][0] = sumLeft  / avgNum;
        avgs[i][1] = sumRight / avgNum;
        
        hitBumper = false;
    }

    char code[1000];
    const char buf[] = "static inline bool isBlue(int red, bool left) {\n\
    if (left) return (%d < red && red < %d);\n\
    else      return (%d < red && red < %d);\n\
}\n\
static inline bool isRed(int red, bool left) {\n\
    if (left) return (%d < red && red < %d);\n\
    else      return (%d < red && red < %d);\n\
}\n\
static inline bool isYellow(int red, bool left) { \n\
    if (left) return (%d < red && red < %d);\n\
    else      return (%d < red && red < %d);\n\
} ";

    sprintf(code, buf, avgs[0][0] - 10, avgs[0][0] + 10, // BLUE
                 avgs[0][1] - 10, avgs[0][1] + 10,

                 avgs[1][1] - 20, avgs[1][1] + 20,

                 avgs[2][0] - 10, avgs[2][0] + 20, // YELLOW
                 avgs[2][1] - 10, avgs[2][1] + 20
                 );

    Serial.println(code);
#endif

    isBot1 = digitalRead(BOT_SWITCH);

    // GO state
    if (isBot1) bot1();
    else        bot2();

    while (1); // stop forever
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


    // Moves backward until red and stops
    actionUntilColor(RED, backward);
    turn(-30); // negative to turn right
    actionUntilColor(RED, turnRight);

    // Flash a red LED
    flashLed(RED_LED); // pauses for 1 second to flash

    // Follows red
    while (!followColorUntilColor(RED, YELLOW)) {}
    Serial.println("Found yellow");

    // Stops on yellow, flashing yellow LED twice
    flashLed(YELLOW_LED); // pauses for 1 second to flash
    flashLed(YELLOW_LED); // pauses for 1 second to flash
    
    // Follows yellow
    Serial.println("Following yellow until red");
    while (!followColorUntilColor(YELLOW, RED)) {}

    // Follows red
    Serial.println("Following red until yellow");
    while (!followColorUntilColor(RED, YELLOW)) {}

    // Stops on yellow, turns on yellow LED, turns 180 degrees
    Serial.println("Turning until red");
    stop();
    digitalWrite(YELLOW_LED, HIGH);
    turn(180);                       // ensures we escape red and actually turn
    actionUntilColor(RED, turnLeft);

    // Follows red
    while (!followColorUntilColor(RED, YELLOW)) {}

    // Communicates to Bot 2: `START`
    delay(500);

    // Waits for `ACK_START`
    delay(500);

    // Flashes green LED
    flashLed(GREEN_LED);

    // Follows red
    while (!followColorUntilColor(YELLOW, RED)) {}

    // Follows red, stops on yellow
    while (!followColorUntilColor(RED, YELLOW)) {}

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

    // Moves backward until blue and stops
    actionUntilColor(BLUE, backward);
    turn(60); // positive to turn left
    actionUntilColor(BLUE, turnLeft);

    // Stops on blue, flashing a blue LED
    flashLed(BLUE_LED);

    // Turns a predetermined angle to the right, and follows blue
    while (!followColorUntilColor(BLUE, YELLOW)) {}
    Serial.println("Found yellow");

    // Stops on yellow, flashing yellow LED twice

    flashLed(YELLOW_LED);
    flashLed(YELLOW_LED);

    // Follows yellow
    Serial.println("Following yellow until blue");
    while (!followColorUntilColor(YELLOW, BLUE)) {}

    // Follows blue
    Serial.println("Following blue until yellow");
    while (!followColorUntilColor(BLUE, YELLOW)) {}

    // Stops on yellow, turns on yellow LED, turns 180 degrees
    Serial.println("Turning until blue");
    stop();
    digitalWrite(YELLOW_LED, HIGH);
    turn(-180);
    actionUntilColor(BLUE, turnRight);

    // Communicates to Bot 1: `TOXIC`
    delay(100);
    // Flash yellow continuously
    // TODO flash continuously

    // Follows blue, stops on yellow
    while (!followColorUntilColor(BLUE, YELLOW)) {}

    // Communicates to Bot 1: `STOP_YELLOW`
    delay(100);

    // Stop flashing yellow LED
    digitalWrite(YELLOW_LED, LOW);

    // Follows yellow until blue
    while (!followColorUntilColor(YELLOW, BLUE)) {}

    // Follows blue, stops on yellow
    while (!followColorUntilColor(BLUE, YELLOW)) {}

    // Communicates to Bot 1: `DONE`
    delay(100);

    // Flashes green LED
    flashLed(GREEN_LED);
}

// ----------------------------------------------------------------------------
// Helper Routines
// ----------------------------------------------------------------------------

inline void flashLed(int ledPin) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
}

inline void actionUntilColor(Colors c, void (*action)(void)) {
    Colors left, right;

    do {
        readSensors(left, right);
        action();
    } while (left != c && right != c);

    stop();
}

inline bool followColorUntilColor(Colors c1, Colors c2) {
    Colors left, right; readSensors(left, right);

    if (left == c2 || right == c2) {
        forward();
        delay(150);
        stop();
        return true;
    }
    else if ((left == c1 && right == c1)) {
        forward();
    }
    else if (left != c1 && right != c1) {
        bool turning = false, turningLeft = true;
        int i = 0, prevMillis;

        do {
            if (!turning) { 
                prevMillis = millis();
                (turningLeft) ? turnLeft() : turnRight();
                turning = true; turningLeft = !turningLeft;
            }

            // turn other way after 100*i milliseconds
            if (millis() > (200 * i) + prevMillis) {
                turning = false; ++i;
            }

            readSensors(left, right);
            if (left == c2 || right == c2) return true;
        } while (left != c1 && right != c1);
        stop();
    }
    else if (left == c1 && right != c1) {
        turnLeft();
        do { readSensors(left, right); } while (right != c1 && right != c2);
        stop();
    }
    else if (left != c1 && right == c1) {
        turnRight();
        do { readSensors(left, right); } while (left != c1 && left != c2);
        stop();
    }
    
    delay(50);

    return false;
}

// ----------------------------------------------------------------------------
// collision.cpp
#define DEBOUNCE_TIME 10000 // ms

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
