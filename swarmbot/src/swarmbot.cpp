#include "pinmap.h"
#include "motor_control.h"
#include "photo_sensor.h"

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

    setupPhotosensor();
    setupMotorControl();
    setupCollision();
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
    while (1) {};
#endif

    //while (!digitalRead(GO_SWITCH)) {} // ON

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

static inline void flashLed(int ledPin) {
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
}
