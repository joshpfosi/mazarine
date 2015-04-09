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

inline bool isBlue  (int red, int blue) { 
    return (850 < red && red < 1000) &&
           (100 < blue && blue < 200);
}

inline bool isRed   (int red, int blue) { 
    return (600 < red && red < 700) &&
           (430 < blue && blue < 600);
}

inline bool isYellow(int red, int blue) { 
    return (400 < red && red < 600) &&
           (150 < blue && blue < 250);
}

inline Colors getColor(int red, int blue) {
    char buf[100]; sprintf(buf, "red=%d, blue=%d", red,blue);
    Serial.println(buf);
    if (isBlue(red, blue))    return BLUE;
    if (isRed(red, blue))     return RED;
    if (isYellow(red, blue))  return YELLOW;
    else                      return BLACK;
}

//
// Read sensor value for red, blue and both
//
inline void readLeftSensor(int *r, int *b) {
    digitalWrite(PHOTO_BLUE_LEFT, LOW);
    digitalWrite(PHOTO_RED_LEFT,  HIGH);
    delay(30);
    *r = analogRead(PHOTOLEFT);
    
    digitalWrite(PHOTO_RED_LEFT,  LOW);
    digitalWrite(PHOTO_BLUE_LEFT, HIGH);
    delay(30);
    *b = analogRead(PHOTOLEFT);

    digitalWrite(PHOTO_BLUE_LEFT, LOW);
}

inline void readRightSensor(int *r, int *b) {
    digitalWrite(PHOTO_BLUE_RIGHT, LOW);
    digitalWrite(PHOTO_RED_RIGHT,  HIGH);
    delay(30);
    *r = analogRead(PHOTORIGHT);
    
    digitalWrite(PHOTO_RED_RIGHT,  LOW);
    digitalWrite(PHOTO_BLUE_RIGHT, HIGH);
    delay(30);
    *b = analogRead(PHOTORIGHT);

    digitalWrite(PHOTO_BLUE_RIGHT, LOW);
}

//
// Test calibration by serial printing colors
//
void test(void) {
    int red, blue;

    readLeftSensor(&red, &blue);
    Colors colorLeft = getColor(red, blue);

    readRightSensor(&red, &blue);
    Colors colorRight = getColor(red, blue);

    delay(100);
    // NOTE assumes Serial is setup via Serial.begin(9600)
    //switch (colorLeft) {
    //    case RED:    Serial.println("Left is red");    break;
    //    case BLUE:   Serial.println("Left is blue");   break;
    //    case YELLOW: Serial.println("Left is yellow"); break;
    //    case BLACK:  Serial.println("Left is black");  break;
    //    default:     Serial.println("We're fucked if we're reading this!");
    //}

    //switch (colorRight) {
    //    case RED:    Serial.println("Right is red");    break;
    //    case BLUE:   Serial.println("Right is blue");   break;
    //    case YELLOW: Serial.println("Right is yellow"); break;
    //    case BLACK:  Serial.println("Right is black");  break;
    //    default:     Serial.println("We're fucked if we're reading this!");
    //}

}

inline bool followColorUntilColor(Colors c1, Colors c2) {
    int redLeft, blueLeft, redRight, blueRight;

    readLeftSensor(&redLeft, &blueLeft);
    readRightSensor(&redRight, &blueRight);

    Colors colorLeft = getColor(redLeft, blueLeft);
    Colors colorRight = getColor(redRight, blueRight);

    if (colorLeft == c2 && colorRight == c2)        {
        stop(); 
        return true;
    }
    else if (colorLeft == c1 && colorRight == c1)   {
        forward();
        return false;
    }
    else if (colorLeft == c1 && colorRight != c1) {
        do { turnLeft();  } while (colorLeft != c1 || colorRight != c1); 
    }
    else if (colorLeft != c1 && colorRight == c1) {
        do { turnRight(); } while (colorLeft != c1 || colorRight != c1); 
    }

    return false;
}

