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

#define PUSH_CONST 300

void bot1(void);
void bot2(void);

// Helper routines
inline void flashLed              (int ledPin);
inline void actionUntilColor      (Colors c, void (*action)(void));
inline bool followColorUntilColor (Colors c1, Colors c2);

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
#if TRANSMIT
    transmit(transMsg, MSG_LEN, TIMEOUT);
#else
    while (!receive(recMsg, MSG_LEN)) {}
    forward();
#endif

#endif

#if TEST_COLLISION
    // check each input for HIGH, setting flag indicating that bumper was hit
    while (1) {
        forward(); delayMicroseconds(100000);
        //if (collisionHappened) {
        //    for (int i = 0; i < NUM_BUMPERS; ++i) {
        //        bumperHit[i] = digitalRead(bumpers[i]);

        //        if (bumperHit[i]) {
                    //switch (i) {
                    //    case 0: forward();   //delayMicroseconds(1000000); stop();
                    //    case 1: backward();  //delayMicroseconds(1000000); stop();
                    //    case 2: turnLeft();  //delayMicroseconds(1000000); stop();
                    //    case 3: turnRight(); //delayMicroseconds(1000000); stop();
                    //    case 4: forward();   //delayMicroseconds(1000000); stop();
                    //}

        //            bumperHit[i] = false;
        //        }
        //    }
        //}
    }
#endif
    
#if CALIBRATION
    bool hitBumper = false;
    int avgs[3][2] = { {0,0}, {0,0}, {0,0} };
    int avgNum = 64;
    for (int i = 0; i < 3; ++i) {
        unsigned sumLeft = 0, sumRight = 0;

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

    sprintf(code, buf, 
                 avgs[0][0] - 20, avgs[0][0] + 20, // BLUE
                 avgs[0][1] - 20, avgs[0][1] + 20,

                 avgs[1][0] - 20, avgs[1][0] + 20, // RED
                 avgs[1][1] - 20, avgs[1][1] + 20,

                 avgs[2][0] - 20, avgs[2][0] + 20, // YELLOW
                 avgs[2][1] - 20, avgs[2][1] + 20
                 );

    Serial.println(code);
    while (1) {};
#endif

    delay(1000);
    isBot1 = digitalRead(BOT_SWITCH);

    // GO state
    if (isBot1) bot1();
    else        bot2();

    digitalWrite(TRANSMIT_PIN, LOW);
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
    turn(-80); // negative to turn right
    actionUntilColor(RED, turnRight);

    // Flash a red LED
    flashLed(RED_LED); // pauses for 1 second to flash

    // Follows red until centre yellow
    while (!followColorUntilColor(RED, YELLOW)) {}
    Serial.println("Found yellow");
    forward();
    delay(PUSH_CONST);
    turn(-20);
    stop();

    // Stops on yellow, flashing yellow LED twice
    flashLed(YELLOW_LED); // pauses for 1 second to flash
    flashLed(YELLOW_LED); // pauses for 1 second to flash

    // experimentally necessary to avoid thick black line
    forward();
    delay(75);
    
    // Follows yellow
    Serial.println("Following yellow until red");
    while (!followColorUntilColor(YELLOW, RED)) {}
    forward();
    delay(PUSH_CONST);
    stop();

    // Follows red
    Serial.println("Following red until yellow");
    while (!followColorUntilColor(RED, YELLOW)) {}

    // Stops on yellow, turns on yellow LED, turns 180 degrees
    Serial.println("Turning until red");
    stop();
    digitalWrite(YELLOW_LED, HIGH);
    turn(220);                       // ensures we escape red and actually turn
    actionUntilColor(RED, turnLeft);

    // Follows red back to the middle
    while (!followColorUntilColor(RED, YELLOW)) {}
    Serial.println("Found yellow and about to communicate");
    forward();
    delay(PUSH_CONST);
    stop();

    // Communicates to Bot 2: `START`
    transmit(startMsg, MSG_LEN, TIMEOUT);

    Serial.println("flashing green LED");
    // Flashes green LED twice
    flashLed(GREEN_LED); // pauses for 1 second to flash
    flashLed(GREEN_LED); // pauses for 1 second to flash

    // Follows yellow
    while (!followColorUntilColor(YELLOW, RED)) {}
    forward();
    delay(PUSH_CONST);
    stop();

    // Follows red, stops on yellow
    while (!followColorUntilColor(RED, YELLOW)) {}

    // Turns on green LED
    digitalWrite(GREEN_LED, HIGH);

    // Waits for `TOXIC`
    Serial.println("Receiving toxic message");
    turnLeft(45);
    while (!receive(recMsg, MSG_LEN)) {}

    stop();
    delay(2 * TIMEOUT); // don't double receive a message

    // Flash yellow LED continuously
    digitalWrite(YELLOW_LED, HIGH);

    // Waits for `STOP_YELLOW`
    turnLeft(45);
    while (!receive(recMsg, MSG_LEN)) {}

    // Turns off yellow LED
    digitalWrite(YELLOW_LED, LOW);

    delay(TIMEOUT); // don't double receive a message

    // Waits for `DONE`
    Serial.println("Receiving toxic message");
    turnLeft(45);
    while (!receive(recMsg, MSG_LEN)) {}
    stop();

    // Flashes green LED
    while (1) { flashLed(GREEN_LED); }
}

void bot2(void) {
    // Waits for `START`
    Serial.println("Waiting for start");
    while (!receive(recMsg, MSG_LEN)) {}

    delay(500);

    // Flash green LED twice
    Serial.println("Lit green led");
    flashLed(GREEN_LED); // pauses for 1 second to flash
    flashLed(GREEN_LED); // pauses for 1 second to flash

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
    turn(80); // positive to turn left
    actionUntilColor(BLUE, turnLeft);

    // Stops on blue, flashing a blue LED
    flashLed(BLUE_LED);

    // Turns a predetermined angle to the right, and follows blue
    while (!followColorUntilColor(BLUE, YELLOW)) {}
    Serial.println("Found yellow");
    forward();
    delay(PUSH_CONST);
    stop();

    // Stops on yellow, flashing yellow LED twice
    flashLed(YELLOW_LED);
    flashLed(YELLOW_LED);

    // Follows yellow
    Serial.println("Following yellow until blue");
    while (!followColorUntilColor(YELLOW, BLUE)) {}
    forward();
    delay(PUSH_CONST);
    stop();

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
    transmit(startMsg, MSG_LEN, TIMEOUT);

    // Flash yellow continuously
    flashLed(YELLOW_LED);
    digitalWrite(YELLOW_LED, HIGH);

    // Follows blue, stops on yellow
    while (!followColorUntilColor(BLUE, YELLOW)) {}
    forward();
    delay(PUSH_CONST);
    stop();

    // Communicates to Bot 1: `STOP_YELLOW`
    transmit(startMsg, MSG_LEN, TIMEOUT);

    // Stop flashing yellow LED
    digitalWrite(YELLOW_LED, LOW);

    // Follows yellow until blue
    while (!followColorUntilColor(YELLOW, BLUE)) {}
    forward();
    delay(PUSH_CONST);
    stop();

    // Follows blue, stops on yellow
    while (!followColorUntilColor(BLUE, YELLOW)) {}
    stop();

    // Communicates to Bot 1: `DONE`
    transmit(doneMsg, MSG_LEN, TIMEOUT);

    // Flashes green LED
    while (1) { flashLed(GREEN_LED); }
}

// ----------------------------------------------------------------------------
// Helper Routines
// ----------------------------------------------------------------------------

inline void flashLed(int ledPin) {
    digitalWrite(ledPin, HIGH);
    delay(100);
    digitalWrite(ledPin, LOW);
    delay(100);
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

    if (left == c2 && right == c2)      { stop(); delay(50); return true; }
    else if (left == c1 && right == c2) turnRight();
    else if (left == c2 && right == c1) turnLeft();
    else if (left == c2 && right == BLACK) forward();
    else if (left == BLACK && right == c2) forward();
    else if (left == c2 && right != c2) turnLeft();
    else if (left != c2 && right == c2) turnRight();
    else if (left == c1 && right == c1) forward();
    else if (left == c1 && right != c1) turnLeft();
    else if (left != c1 && right == c1) turnRight();
    else if (left != c1 && right != c1) {
        Serial.println("Both off c1");
        bool turning = false, turningLeft = !isBot1;
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

    delay(150);
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

        digitalWrite(13, HIGH);
        for (int i = 0; i < NUM_BUMPERS; ++i) {
            if (digitalRead(bumpers[i])) {
                switch (i) {
                    case 0: forward();   delayMicroseconds(500000); stop();
                    case 1: backward();  delayMicroseconds(500000); stop();
                    case 2: turnLeft();  delayMicroseconds(500000); stop();
                    case 3: turnRight(); delayMicroseconds(500000); stop();
                    case 4: forward();   delayMicroseconds(500000); stop();
                }
            }
        }
    }
}

void setupCollision(void) {
    attachInterrupt(COLLISION_INT, detectCollision, RISING);
    for (int i = 0; i < NUM_BUMPERS; ++i) pinMode(bumpers[i], INPUT);
}
// ----------------------------------------------------------------------------
