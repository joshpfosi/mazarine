// photosensor.cpp
// Exports helper functions for line following and color detection
// Assumes pins have been properly setup for IN/OUTPUT

#include "pinmap.h"
#include "photo_sensor.h"

void setupPhotosensor(void) {
    pinMode(PHOTOLEFT,        INPUT);
    pinMode(PHOTORIGHT,       INPUT);

    // red can be on at all times
#if MAZ
    pinMode(PHOTO_RED_LEFT,   OUTPUT);
    pinMode(PHOTO_RED_RIGHT,  OUTPUT);
    digitalWrite(PHOTO_RED_LEFT,  HIGH);
    digitalWrite(PHOTO_RED_RIGHT, HIGH);
#else // Erythrean uses blue not red
    pinMode(PHOTO_BLUE_LEFT,  OUTPUT);
    digitalWrite(PHOTO_BLUE_LEFT,  HIGH);
#endif
    
}

#if MAZ
static inline bool isBlue(int red, bool left) {
    if (left) return (977 < red && red < 997);
    else      return (984 < red && red < 1004);
}
static inline bool isRed(int red, bool left) {
    if (left) return (820 < red && red < 870);
    else      return (866 < red && red < 920);
}
static inline bool isYellow(int red, bool left) {
    if (left) return (700 < red && red < 779);
    else      return (800 < red && red < 852);
}   
#else
static inline bool isBlue(int red, bool left) {
    if (left) return (740 < red && red < 880);
    else      return (570 < red && red < 700);
}
static inline bool isRed(int red, bool left) {
    if (left) return (150 < red && red < 250);
    else      return (110 < red && red < 200);
}
static inline bool isYellow(int red, bool left) {
    if (left) return (450 < red && red < 550);
    else      return (350 < red && red < 450);
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

