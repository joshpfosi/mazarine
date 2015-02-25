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
    return (900 < red && red < 1000) &&
           (100 < blue && blue < 200);
}

static inline bool isRed   (int red, int blue) { 
    return (600 < red && red < 700) &&
           (500 < blue && blue < 600);
}

static inline bool isYellow(int red, int blue) { 
    return (500 < red && red < 600) &&
           (150 < blue && blue < 250);
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

    Serial.begin(9600);
}

void loop() { 
    int red, blue;

    //
    // Read sensor value for red, blue and both
    //
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(RED_LED,  HIGH);
    delay(30);
    red = analogRead(PHOTO_SENSOR);
    
    digitalWrite(RED_LED,  LOW);
    digitalWrite(BLUE_LED, HIGH);
    delay(30);
    blue = analogRead(PHOTO_SENSOR);

    digitalWrite(BLUE_LED, LOW);
    
    //char msg[100];
    //sprintf(msg, "red = %d, blue = %d\n", red, blue);
    //Serial.print(msg);
    
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

    //forward();
    //if (isBlue(red, blue)) stop();

    ////
    //// 2. Follow a straight blue strip
    ////

    //while (1) {
    //    // correct path
    //    if (!isBlue(red, blue)) {
    //        // turn left a small amt, and if blue, go straight, else go right
    //        turn(10);
    //        red = analogRead(PHOTO_SENSOR);
    //    }
    //}
    //
    ////
    //// 3. Follow an arcing blue strip of angle 90 degrees
    ////

    //
    // 4. Demonstrate detection of colors via turning
    //
    
    //if (isRed(red, blue))    turn(90);
    //if (isBlue(red, blue))   turn(-90);
    //if (isYellow(red,blue))  turn(180);
}

