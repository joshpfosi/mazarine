/* photosensor_test.ino
 * Flashes LEDs to test photosensor and color detection
 */

#include "motor_control.h"

#define RED_LED      12
#define BLUE_LED     13
#define RED_IND      5
#define BLUE_IND     6
#define YELLOW_IND   7
#define PHOTO_SENSOR A0

static inline bool isBlue  (int red, int blue) { 
    return (850 < red && red < 1000) &&
           (100 < blue && blue < 200);
}

static inline bool isRed   (int red, int blue) { 
    return (600 < red && red < 700) &&
           (430 < blue && blue < 600);
}

static inline bool isYellow(int red, int blue) { 
    return (400 < red && red < 600) &&
           (150 < blue && blue < 250);
}

//
// Read sensor value for red, blue and both
//
static inline void readSensor(int *r, int *b) {
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(RED_LED,  HIGH);
    delay(30);
    *r = analogRead(PHOTO_SENSOR);
    
    digitalWrite(RED_LED,  LOW);
    digitalWrite(BLUE_LED, HIGH);
    delay(30);
    *b = analogRead(PHOTO_SENSOR);

    digitalWrite(BLUE_LED, LOW);
}

void setup() {
    // set up sensor LEDs for outputs
    pinMode(RED_LED,      OUTPUT);
    pinMode(BLUE_LED,     OUTPUT);
    pinMode(PHOTO_SENSOR, INPUT);
    pinMode(RED_IND,      OUTPUT);
    pinMode(BLUE_IND,     OUTPUT);
    pinMode(YELLOW_IND,   OUTPUT);

    // set up motor pins for OUTPUT 
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(E1,  OUTPUT);
    pinMode(E2,  OUTPUT);
}

// global used for 1st test
static bool sawBlue = false;

void loop() { 
    int red, blue;

    //
    // 0. Test by indicator LEDs
    //

    //int r = isRed(red, blue), b = isBlue(red, blue), y = isYellow(red,blue);
    //if (r)              Serial.print("I'm red");
    //if (b)              Serial.print("I'm blue");
    //if (y)              Serial.print("I'm yellow");
    //if (!r && !b && !y) Serial.print("I'm black");

    // 
    // 1. Move forward until blue tape
    //

    //readSensor(&red, &blue);

    //if (sawBlue)
    //    stop();
    //else {
    //    sawBlue = isBlue(red, blue);
    //    forward();
    //}

    //
    // 2. Follow a straight blue strip
    //

    readSensor(&red, &blue);

    bool turning = false, left = true;
    int i, prevMillis;

    i = 1;
    while (!isBlue(red, blue)) {
        // turn right or left
        if (!turning) { 
            prevMillis = millis();
            (left) ? turnLeft() : turnRight();
            turning = true; left = !left;
        }

        // if turned back and forth this much, blue must be gone so stop 
        while (i > 10) stop();

        // turn other way after 100*i milliseconds
        if (millis() > (100 * i) + prevMillis) {
            turning = false; ++i;
        }

        readSensor(&red, &blue);
    }

    // invariant: if here, we've found blue
    forward();
    
    //
    // 4. Demonstrate detection of colors via turning
    //

    //forward();
    //readSensor(&red, &blue);
    //
    //if (isRed(red, blue))    turn(90);
    //if (isBlue(red, blue))   turn(-90);
    //if (isYellow(red,blue))  turn(180);
    //
    //forward();
}

