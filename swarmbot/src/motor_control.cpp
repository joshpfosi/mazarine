/* motor_control.c
 *
 * Basic concept is that forward/backward/stop are stateless, and simply set
 * the hardware in motion, while turning is handled internally, preventing
 * any other bot functionality while it is being executed
 * 
 * The bot speed is held in a static global to avoid the overhead of structs
 */

#include "Arduino.h"
#include "motor_control.h"

/* -------------------------- PRIVATE -------------------------- */

#if MAZ

#define SPEED_CONST 70
#define LEFT_CALIB  1
#define RIGHT_CALIB 1

#else

#define SPEED_CONST 100
#define LEFT_CALIB  1.2
#define RIGHT_CALIB 1

#endif

#define TURN_CONST  5 // found experimentally

void setupMotorControl(void) {
    pinMode(ML1, OUTPUT);
    pinMode(ML2, OUTPUT);
    pinMode(MR1, OUTPUT);
    pinMode(MR2, OUTPUT);

    pinMode(EL, OUTPUT);
    pinMode(ER, OUTPUT);
}

/* initialize to some speed */
static int speed = SPEED_CONST;

static void enable(int s)  { 
    analogWrite(EL, LEFT_CALIB  * s);
    analogWrite(ER, RIGHT_CALIB * s);
}

static void disable(void) { analogWrite(EL, 0); analogWrite(ER, 0);     }

/* -------------------------- INTERFACE -------------------------- */

/* going to need an init function to initialize pins and speed */

void set_speed(int s) { speed = s;    }
int  get_speed(void)  { return speed; }

void forward(void) {
    disable();

    digitalWrite(ML1, HIGH);
    digitalWrite(ML2, LOW);

    digitalWrite(MR1, HIGH);
    digitalWrite(MR2, LOW);

    enable(SPEED_CONST);
}

void backward(void) {
    disable();

    digitalWrite(ML1, LOW);
    digitalWrite(ML2, HIGH);

    digitalWrite(MR1, LOW);
    digitalWrite(MR2, HIGH);

    enable(2 * SPEED_CONST);
}

// turn both motors in opposite directions to pivot
void turn(int angle) {
    disable();
    
    /* these constants were found experimentally */
    int turn_constant = TURN_CONST;

    if (angle > 0) { // turn left
        // turn left motor forward
        digitalWrite(ML1, HIGH);
        digitalWrite(ML2, LOW);

        // turn right motor backward
        digitalWrite(MR1, LOW);
        digitalWrite(MR2, HIGH);
    } else { // turn right
        // turn left motor backward
        digitalWrite(ML1, LOW);
        digitalWrite(ML2, HIGH);

        // turn right motor forward
        digitalWrite(MR1, HIGH);
        digitalWrite(MR2, LOW);
    }

    enable(SPEED_CONST);

    angle = (angle > 0) ? angle : 0 - angle; // abs(angle)
    delay(angle * turn_constant);            // measured constant
    
    disable();
}

void turnRight(void) {
    disable();

    // turn left motor forward
    digitalWrite(ML1, LOW);
    digitalWrite(ML2, HIGH);

    // turn right motor backward
    digitalWrite(MR1, HIGH);
    digitalWrite(MR2, LOW);

    enable(50);
}

void turnLeft(void) {
    disable();

    // turn left motor backward
    digitalWrite(ML1, HIGH);
    digitalWrite(ML2, LOW);

    // turn right motor forward
    digitalWrite(MR1, LOW);
    digitalWrite(MR2, HIGH);

    enable(50);
}

void stop(void) { disable(); }

