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
}

bool isBlue(int red, int blue, bool left) { 
    if (left) return (900 < red && red < 1024) && (500 < blue && blue < 650);
    else      return (900 < red && red < 1024) && (150 < blue && blue < 250);
}

bool isRed(int red, int blue, bool left) { 
    if (left) return (700 < red && red < 800) && (900 < blue && blue < 1024);
    else      return (800 < red && red < 900) && (650 < blue && blue < 750);
}

bool isYellow(int red, int blue, bool left) { 
    if (left) return (600 < red && red < 700) && (750 < blue && blue < 850);
    else      return (700 < red && red < 800) && (150 < blue && blue < 250);
}

//
// Read sensor value for red, blue and both
//
Colors readLeftSensor(void) {
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

Colors readRightSensor(void) {
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

void actionUntilColor(Colors c, void(*action)(void)) {
    while (readLeftSensor() != c && readRightSensor() != c) action();
    stop();
}

bool followColorUntilColor(Colors c1, Colors c2) {
    Colors colorLeft  = readLeftSensor();
    Colors colorRight = readRightSensor();

    if (colorLeft == c2 && colorRight == c2)        {
        stop(); 
        return true;
    }
    else if (colorLeft != c2 && colorRight == c2) {
        turnRight();
        do {
            colorLeft = readLeftSensor();
        } while (colorLeft != c2);

        stop();
    }
    else if (colorLeft == c2 && colorRight != c2) {
        turnLeft();
        do {
            colorRight = readRightSensor();
        } while (colorRight != c2);

        stop();
    }
    else if ((colorLeft == c1 && colorRight == c1)) {
        Serial.println("Both on color ");
        forward();
        return false;
    }
    else if (colorLeft != c1 && colorRight != c1) {
        Serial.println("Both off color ");
        bool turning = false, left = true;
        int i = 0, prevMillis;

        do {
            if (!turning) { 
                prevMillis = millis();
                (left) ? turnLeft() : turnRight();
                turning = true; left = !left;
            }

            // turn other way after 100*i milliseconds
            if (millis() > (200 * i) + prevMillis) {
                turning = false; ++i;
            }

            colorLeft  = readLeftSensor();
            colorRight = readRightSensor();
            if (colorLeft == c2 || colorRight == c2) return true;
        } while (colorLeft != c1 && colorRight != c1);
    }
    else if (colorLeft == c1 && colorRight != c1) {
        turnLeft();
        do {
            Serial.println("Right is off");
            colorRight = readRightSensor();
        } while (colorRight != c1 && colorRight != c2);
        stop();
    }
    else if (colorLeft != c1 && colorRight == c1) {
        turnRight();
        do {
            Serial.println("Left is off");
            colorLeft  = readLeftSensor();
        } while (colorLeft != c1 && colorLeft != c2);
        stop();
    }

    return false;
}

