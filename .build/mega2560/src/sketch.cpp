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
#define FALSE         0
#define TRUE          1

/* pins */
#define RED_LED       6
#define BLUE_LED      4
#define GREEN_LED     8
#define PIN1          0
#define PIN2          1
#define POTPIN1       A0
#define POTPIN2       A1

/* states */
#define ON            0
#define OFF           1
#define RUN           2
#define SLEEP         3
#define DIAG          4

/* misc */
#define ONE_SEC_DELAY 20
#define LED_ON        255
#define LED_OFF       0
#define SLEEP_DUR     15000

/* frequencies */
#define TEN_HERTZ     50
#define FOUR_HERTZ    125
#define TWO_HERTZ     250
#define ONE_HERTZ     500

/* global which tracks diagnostic errors */
int numErrors = 5;

/* contains interrupt state */
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

/* ISRs */
void setSwitch1(void);
void setSwitch2(void);

void setup() {
    pinMode(RED_LED,   OUTPUT);
    pinMode(BLUE_LED,  OUTPUT);
    pinMode(GREEN_LED, OUTPUT);
    pinMode(POTPIN1,   INPUT);
    pinMode(POTPIN2,   INPUT);
    attachInterrupt(PIN1, setSwitch1, RISING); // NOTE should be FALLING
    attachInterrupt(PIN2, setSwitch2, RISING);
}

int curState = ON;

void loop() {
    curState = state[curState]();
    if (curState != RUN) digitalWrite(RED_LED, LOW);
}

int on(void) {
    digitalWrite(RED_LED, HIGH); delay(ONE_HERTZ);
    digitalWrite(RED_LED, LOW);  delay(ONE_HERTZ);

    return DIAG;
}

int off(void) {
    return OFF;
}

int run(void) {
    if (millis() > SLEEP_DUR) return SLEEP;

    float brightness = LED_ON; 
    long prevMillisGreen = millis(), prevMillisBlue = millis(), currentMillis = 0;
    int fadeAmt = 5, blueLedStatus = 0, freq;
    float pot1Val = 0, pot2Val = 0;

    while (1) {
        currentMillis = millis();
        pot1Val = analogRead(POTPIN1) / 500.0;
        pot2Val = (analogRead(POTPIN2) / 1023.0) * LED_ON;

        long greenInterval = currentMillis - prevMillisGreen;

        if (greenInterval > 120 && brightness > 0) {
            prevMillisGreen = currentMillis;
            brightness -= fadeAmt;
            analogWrite(GREEN_LED, brightness);    
        } 
        else if (brightness <= 0) {
            if (greenInterval > 2000) {
                analogWrite(GREEN_LED, LED_OFF); return RUN;
            }
            else if (greenInterval > 1500) { analogWrite(GREEN_LED, LED_OFF); }
            else if (greenInterval > 1000) { analogWrite(GREEN_LED, LED_ON);  }
            else if (greenInterval > 500)  { analogWrite(GREEN_LED, LED_OFF); }
            else if (greenInterval > 0)    { analogWrite(GREEN_LED, LED_ON);  }
        }

        /* blue LED */
        long blueInterval = currentMillis - prevMillisBlue;
        freq = (switch1) ? TEN_HERTZ : ONE_HERTZ;
        freq *= pot1Val;
        if (blueInterval > freq) {
            prevMillisBlue = currentMillis;
            blueLedStatus = (blueLedStatus == 0) ? LED_ON : LED_OFF;
            analogWrite(BLUE_LED, blueLedStatus);
        }
        /* reduce brightness by number between 0 and 255 */
        if (switch2) analogWrite(RED_LED, LED_ON - pot2Val);

    }

    return RUN;
}

int sleep(void) {
    int i;
    for (i = 0; i < 4; ++i) {
        analogWrite(BLUE_LED, LED_ON);  delay(FOUR_HERTZ);
        analogWrite(BLUE_LED, LED_OFF); delay(FOUR_HERTZ);
    }

    int brightness = LED_ON, fadeAmt = 5; 

    // fade LED
    while (brightness > 0) {
        analogWrite(BLUE_LED, brightness);    
        brightness -= fadeAmt;
        delay(20);    
    }

    analogWrite(BLUE_LED, LED_OFF);

    return OFF;
}

int diag(void) {
    int i;
    for (i = 0; i < numErrors; ++i) {
        digitalWrite(RED_LED, HIGH); delay(TWO_HERTZ);
        digitalWrite(RED_LED, LOW);  delay(TWO_HERTZ);
    }

    return RUN;
}

/* ---------- ISRs ---------- */

void setSwitch1(void) { switch1 = true; }
void setSwitch2(void) { switch2 = switch1; }
