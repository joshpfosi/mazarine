#include <Arduino.h>

void setup();
void loop();
int on(void);
int off(void);
int run(void);
int sleep(void);
int diag(void);
void setSwitch1(void);
void setSwitch2(void);
#line 1 "src/sketch.ino"
#define RED_LED       6
#define BLUE_LED      4
#define GREEN_LED     8
#define PIN1          0
#define PIN2          1
#define TIMER         12
#define ON            0
#define OFF           1
#define RUN           2
#define SLEEP         3
#define DIAG          4
#define ONE_SEC_DELAY 20
#define FALSE         0
#define TRUE          1

/* global which tracks diagnostic errors */
int numErrors = 5;
int timer     = 0;
int switch1   = FALSE;
int switch2   = FALSE;

/* state computation */
int on    (void);
int off   (void);
int run   (void);
int sleep (void);
int diag  (void);

/* states */
int (* state[])(void) = { on, off, run, sleep, diag };
//enum states { ON, OFF, RUN, SLEEP, DIAG, MAX_STATES } currentState;

/* ISRs */
void setSwitch1(void);
void setSwitch2(void);

void setup() {
    pinMode(RED_LED,   OUTPUT);
    pinMode(BLUE_LED,  OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    attachInterrupt(PIN1, setSwitch1, RISING); // NOTE should be FALLING
    attachInterrupt(PIN2, setSwitch2, RISING);
}

int curState = ON;

void loop() {
    curState = state[curState]();
    if (curState != RUN) analogWrite(RED_LED, 0);
    timer++;
}

int on(void) {
    digitalWrite(RED_LED, HIGH);
    delay(1000);
    digitalWrite(RED_LED, LOW);
    delay(1000);

    return DIAG;
}

int off(void) {
    return OFF;
}

int run(void) {
    if (timer == TIMER) return SLEEP;

    float brightness = 255.0; 
    // NUM_BLINKS 60 or 6
    // DELAY ONE_HERTZ TEN_HERTZ

    // fade LED
    if (!switch1) {
        float fadeAmount = brightness / 5; // NOTE this is sketchy

        while (brightness > 0) {
            analogWrite(GREEN_LED, brightness);    
            brightness -= fadeAmount;
            analogWrite(BLUE_LED, 255);
            delay(1000);    

            analogWrite(GREEN_LED, brightness);    
            brightness -= fadeAmount;
            analogWrite(BLUE_LED, 0);
            delay(1000);    
        }

        analogWrite(BLUE_LED, 255);
        // blink twice at 2 Hz
        analogWrite(GREEN_LED, 255); delay(500);
        analogWrite(GREEN_LED, 0);   delay(500);

        analogWrite(BLUE_LED, 0);

        analogWrite(GREEN_LED, 255); delay(500);
        analogWrite(GREEN_LED, 0);   delay(500);
    } 
    else {
        float fadeAmount = brightness / 60;

        while (brightness > 0) {
            if (switch2) analogWrite(RED_LED, 255);

            analogWrite(GREEN_LED, brightness);    
            brightness -= fadeAmount;
            analogWrite(BLUE_LED, 255);
            delay(100);    

            analogWrite(GREEN_LED, brightness);    
            brightness -= fadeAmount;
            analogWrite(BLUE_LED, 0);
            delay(100);    
        }

        analogWrite(GREEN_LED, 255);
        analogWrite(BLUE_LED, 255);  delay(100);
        analogWrite(BLUE_LED, 0);    delay(100);
        analogWrite(BLUE_LED, 255);  delay(100);
        analogWrite(BLUE_LED, 0);    delay(100);
        analogWrite(BLUE_LED, 255);  delay(100);
        analogWrite(GREEN_LED, 0);
        analogWrite(BLUE_LED, 255);  delay(100);
        analogWrite(BLUE_LED, 0);    delay(100);
        analogWrite(BLUE_LED, 255);  delay(100);
        analogWrite(BLUE_LED, 0);    delay(100);
        analogWrite(BLUE_LED, 255);  delay(100);
    }

    return RUN;
}

int sleep(void) {
    int i;
    for (i = 0; i < 4; ++i) {
        analogWrite(BLUE_LED, 255); delay(250);
        analogWrite(BLUE_LED, 0);   delay(250);
    }

    int brightness = 255, fadeAmount = 5; 

    // fade LED
    while (brightness > 0) {
        analogWrite(BLUE_LED, brightness);    
        brightness -= fadeAmount;
        delay(ONE_SEC_DELAY);    
    }

    analogWrite(BLUE_LED, 0);

    return OFF;
}

int diag(void) {
    int i;
    for (i = 0; i < numErrors; ++i) {
        // TODO should be 500
        digitalWrite(RED_LED, HIGH); delay(200);
        digitalWrite(RED_LED, LOW);  delay(200);
    }

    return RUN;
}

/* ---------- ISRs ---------- */

void setSwitch1(void) { switch1 = true; }
void setSwitch2(void) { switch2 = switch1; }
