#include "pinmap.h"
#include "photo_sensor.h"
#include "motor_control.h"

// ----------------------------------------------------------------------------
// State variables
static bool isBot1  = true;
static bool hitWall = false;
// ----------------------------------------------------------------------------

void bot1(void);
void bot2(void);

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
// ----------------------------------------------------------------------------

void setup() {
    // ------------------------------------------------------------------------
    // Set up general Bot pins
    //pinMode( GO_SWITCH,  INPUT);
    //pinMode( BOT_SWITCH, INPUT);
    // ------------------------------------------------------------------------

    Serial.begin(9600);

    setupPhotosensor();
    setupMotorControl();

    attachInterrupt(COLLISION_INT, detectCollision, CHANGE);
    for (int i = 0; i < NUM_BUMPERS; ++i) pinMode(bumpers[i], INPUT);
}

void loop() {
    // TEST COLLISION
    //if (collisionHappened) {
    //    for (int i = 0; i < NUM_BUMPERS; ++i) {
    //        bumperHit[i] = digitalRead(bumpers[i]);
    //
    //        if (bumperHit[i]) {
    //            Serial.println(i);
    //            bumperHit[i] = false;
    //        }
    //    }
    //}

    // TEST MOTOR CONTROL
    //forward();
    //delay(1000);
    //backward();
    //delay(2000);
    //stop();
    //delay(500);



    //while (!digitalRead(GO_SWITCH)) { delayMicroseconds(1); } // ON

    isBot1 = true; //digitalRead(BOT_SWITCH);

    // GO state
    if (isBot1) bot1();
    else        bot2();
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


    // Assumes it hit the correct wall and turns a predetermined angle to the left

    stop(); delay(1000); // resolve collision by backing up
    forward();
    turn(-20); // negative to turn left

    // Moves forward 
    forward();

    // Stops on red, flashing a red LED
    int sensorL;
    int sensorR;
    do {
      int r, b;
      readRightSensor(r, b);
      sensorL = getColor(r, b);
      readLeftSensor(r, b);
      sensorR = getColor(r, b);
      forward();      
    } while (sensorL != RED && sensorR != RED);
    stop();
    flashLED(RED_LED);
    
//    // Stops on red, flashing a red LED
//    // Turns a predetermined angle to the right, and follows red
//    // Stops on yellow, flashing yellow LED twice
//    // Moves forward
//    // Follows red
//    // Stops on yellow, turns on yellow LED, turns 180 degrees
//    // Moves forward until red
//    // Follows red
//    // Stops on yellow
//    // Communicates to Bot 2: `START`
//    // Waits for `ACK_START`
//    // Flashes green LED
//    // Moves forward until red
//    // Follows red
//    // TBD // we want this to be close to a specific location
//    // Turns on green LED
//    // Waits for `TOXIC`
//    // Flash yellow LED continuously
//    // Waits for `STOP_YELLOW`
//    // Turns off yellow LED
//    // Waits for `DONE`
//    // Flashes green LED
}

void flashLED(int ledPin) {
  digitalWrite(ledPin, HIGH);
  delay(100);
  digitalWrite(ledPin, LOW);
  delay(100);
}

// NEED TO BE MOVED TO PINMAP.H
int ledState = LOW;
long previousMillis = 0;
long interval = 1000;

void flashingLED(int ledPin) {
  while(1) {
    static unsigned long currentMillis = millis();
    if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;   
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
    }
  }
}

void stopOn(Colors color) {
  static int sensorL, sensorR, r, b;
  do {
    readRightSensor(r, b);
    sensorL = getColor(r, b);
    readLeftSensor(r, b);
    sensorR = getColor(r, b);
    forward();
  } while (sensorL != color || sensorR != color);
  stop();
}

void bot2(void) {
//    // Waits for `START`
  //static int  msg;
  //msg = receive();
  //while(! START) delayMicroseconds(1);
//    // Flash green LED
  flashLED(GREEN_LED);
//    // Puts bot in motion (`forward()`)
//    // Loops until collision (boolean is set in ISR)
  while (!hitWall) {
    forward();
    for (int i = 0; i < NUM_BUMPERS; ++i) {
      if (digitalRead(bumpers[i])) hitWall = true;
    }
    delayMicroseconds(1);
  }
//    // Assumes it hit the correct wall and turns a predetermined angle to the left
  turn(-160);
//    // Moves forward 
//    // Stops on blue, flashing a blue LED
  stopOn(BLUE);  // JUST REALIZE THAT WE MIGHT NOT NEED THIS IF WE ADD THE !c1 && !c2 forward() CONDITION IN followColorUntilColor
  flashLED(BLUE_LED);
//    // Turns a predetermined angle to the right, and follows blue
  turn(160);
//    // Stops on yellow, flashing yellow LED twice
  followColorUntilColor(BLUE, YELLOW);
  flashLED(YELLOW_LED);
  flashLED(YELLOW_LED);
//    // Moves forward
//    // Follows blue
  followColorUntilColor(BLUE, YELLOW);
//    // Stops on yellow, turns on yellow LED, turns 180 degrees
  turn(180);
//    // Communicates to Bot 1: `TOXIC`
  //trasmit(TOXIC);
//    // Flash yellow continuously
  flashingLED(YELLOW_LED);
//    // Moves forward until blue
  stopOn(BLUE);
//    // Follows blue
//    // Stops on yellow
  followColorUntilColor(BLUE, YELLOW);
//    // Communicates to Bot 1: `STOP_YELLOW`
  //transmit(STOP_YELLOW);
//    // Stop flashing yellow LED
  digitalWrite(YELLOW_LED, LOW);
//    // Moves forward until blue
  stopOn(BLUE);
//    // Follows blue
//    // TBD // we want this to be close to a specific location
//    // Communicates to Bot 1: `DONE`
  //transmit(DONE);
//    // Flashes green LED
  flashLED(GREEN_LED);
}
