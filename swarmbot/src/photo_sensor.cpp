// photosensor.cpp
// Exports helper functions for line following and color detection
// Assumes pins have been properly setup for IN/OUTPUT

#include "pinmap.h"
#include "photo_sensor.h"
#include "motor_control.h"

void setupPhotosensor(void) {
    pinMode(PHOTO_RED_LEFT,   OUTPUT);
    pinMode(PHOTO_RED_RIGHT,  OUTPUT);
    pinMode(PHOTO_BLUE_LEFT,  OUTPUT);
    pinMode(PHOTO_BLUE_RIGHT, OUTPUT);
    pinMode(PHOTOLEFT,        INPUT);
    pinMode(PHOTORIGHT,       INPUT);

    // red can be on at all times
    digitalWrite(PHOTO_RED_LEFT,  HIGH);
    digitalWrite(PHOTO_RED_RIGHT, HIGH);
}

static inline bool isBlue(int red, bool left) { 
    if (left) return (970 < red && red < 980);
    else      return (985 < red && red < 995);
}

static inline bool isRed(int red, bool left) { 
    if (left) return (700 < red && red < 800);
    else      return (800 < red && red < 900);
}

static inline bool isYellow(int red, bool left) { 
    if (left) return (640 < red && red < 690);
    else      return (770 < red && red < 800);
}

//
// Read sensor value for red, blue and both
//
void readSensors(Colors &left, Colors &right) {
    int rR, rL;

    rL = analogRead(PHOTOLEFT);
    rR = analogRead(PHOTORIGHT);

    //char buf[100]; 
    //sprintf(buf, "LEFT:  red=%d",rL);
    //Serial.println(buf);

    //sprintf(buf, "RIGHT: red=%d", rR);
    //Serial.println(buf);

    if (isBlue(rR,        false)) right = BLUE;
    else if (isRed(rR,    false)) right = RED;
    else if (isYellow(rR, false)) right = YELLOW;
    else                          right = BLACK;

    if (isBlue(rL,        true)) left = BLUE;
    else if (isRed(rL,    true)) left = RED;
    else if (isYellow(rL, true)) left = YELLOW;
    else                         left = BLACK;
}

//
// Test calibration by serial printing colors
//
void testPhotosensor(void) {
    Colors left, right; readSensors(left, right);

    delay(500);
    // NOTE assumes Serial is setup via Serial.begin(9600)
    switch (left) {
        case RED:    Serial.println("Left is red");    break;
        case BLUE:   Serial.println("Left is blue");   break;
        case YELLOW: Serial.println("Left is yellow"); break;
        case BLACK:  Serial.println("Left is black");  break;
        default:     Serial.println("We're fucked if we're reading this!");
    }

    switch (right) {
        case RED:    Serial.println("Right is red");    break;
        case BLUE:   Serial.println("Right is blue");   break;
        case YELLOW: Serial.println("Right is yellow"); break;
        case BLACK:  Serial.println("Right is black");  break;
        default:     Serial.println("We're fucked if we're reading this!");
    }
}

void actionUntilColor(Colors c, void (*action)(void)) {
    Colors left, right;

    do {
        readSensors(left, right);
        action();
    } while (left != c && right != c);

    stop();
}

bool followColorUntilColor(Colors c1, Colors c2) {
    Colors left, right; readSensors(left, right);

    if (left == c2 && right == c2) {
        stop(); return true;
    }
    else if (left != c2 && right == c2) {
        turnRight();
        do { readSensors(left, right); } while (left != c2 && right == c2);
        stop();
    }
    else if (left == c2 && right != c2) {
        turnLeft();
        do { readSensors(left, right); } while (right != c2 && left == c2);
        stop();
    }
    else if ((left == c1 && right == c1)) {
        forward(); return false;
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
            if (left == c2 || right == c2) return false;
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

    return false;
}

