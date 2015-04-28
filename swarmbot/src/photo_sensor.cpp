// photosensor.cpp
// Exports helper functions for line following and color detection
// Assumes pins have been properly setup for IN/OUTPUT

#include "pinmap.h"
#include "photo_sensor.h"

void setupPhotosensor(void) {
    pinMode(PHOTO_RED_LEFT,   OUTPUT);
    pinMode(PHOTO_RED_RIGHT,  OUTPUT);
    pinMode(PHOTO_BLUE_LEFT,  OUTPUT);
    pinMode(PHOTO_BLUE_RIGHT, OUTPUT);
    pinMode(PHOTOLEFT,        INPUT);
    pinMode(PHOTORIGHT,       INPUT);

    // red can be on at all times
#if MAZ
    digitalWrite(PHOTO_RED_LEFT,  HIGH);
    digitalWrite(PHOTO_RED_RIGHT, HIGH);
#else // Erythrean uses blue not red
    digitalWrite(PHOTO_BLUE_LEFT,  HIGH);
    digitalWrite(PHOTO_BLUE_RIGHT, HIGH);
#endif
    
}

#if MAZ
static inline bool isBlue(int red, bool left) {
    if (left) return (974 < red && red < 994);
    else      return (982 < red && red < 1002);
}
static inline bool isRed(int red, bool left) {
    if (left) return (796 < red && red < 846);
    else      return (852 < red && red < 892);
}
static inline bool isYellow(int red, bool left) {
    if (left) return (725 < red && red < 765);
    else      return (803 < red && red < 833);
}
#else
static inline bool isBlue(int red, bool left) {
    if (left) return (815 < red && red < 844);
    else      return (650 < red && red < 720);
}
static inline bool isRed(int red, bool left) {
    if (left) return (200 < red && red < 250);
    else      return (140 < red && red < 200);
}
static inline bool isYellow(int red, bool left) {
    if (left) return (540 < red && red < 630);
    else      return (380 < red && red < 500);
}
#endif


//
// Read sensor value for red, blue and both
//
void readSensors(Colors &left, Colors &right) {
    int rR, rL;

    rL = analogRead(PHOTOLEFT);
    rR = analogRead(PHOTORIGHT);

#if TEST_PHOTO
    char buf[100]; 
    sprintf(buf, "LEFT:  red=%d",rL);
    Serial.println(buf);

    sprintf(buf, "RIGHT: red=%d", rR);
    Serial.println(buf);
#endif

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

