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

#define MAX_SPEED 5
#define MIN_SPEED 1 

void setupMotorControl(void) {
    pinMode(ML1, OUTPUT);
    pinMode(ML2, OUTPUT);
    pinMode(MR1, OUTPUT);
    pinMode(MR2, OUTPUT);

    pinMode(EL, OUTPUT);
    pinMode(ER, OUTPUT);
}

/* initialize to some speed */
static int speed = 1;

/* converts angular velocity to pwm value via some experimentally 
 * found constant
 */
static int speed_to_pwm(int s) {
    if (s < MIN_SPEED || s > MAX_SPEED) s = 3;
    return 70 * s; /* minimum is 0, max is 255 */
}

static void enable(int forward)  { 
    if (forward) {
        analogWrite(EL, speed_to_pwm(speed));
        analogWrite(ER, speed_to_pwm(speed));
    }
    else { /* backward */
        analogWrite(EL, speed_to_pwm(speed));
        analogWrite(ER, speed_to_pwm(speed));
    }
}

static void disable(void) { analogWrite(EL, 0);     analogWrite(ER, 0);     }

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

    enable(true);
}

void backward(void) {
    disable();

    digitalWrite(ML1, LOW);
    digitalWrite(ML2, HIGH);

    digitalWrite(MR1, LOW);
    digitalWrite(MR2, HIGH);

    enable(false);
}

// turn both motors in opposite directions to pivot
void turn(int angle) {
    disable();
    
    /* these constants were found experimentally */
    int s = 70, turn_constant = 4;

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

    analogWrite(EL, s);
    analogWrite(ER, s);

    angle = (angle > 0) ? angle : 0 - angle; // abs(angle)
    delay(angle * turn_constant);            // measured constant
    
    disable();
}

void turnRight(void) {
    disable();
    
    int s = 40;

    // turn left motor forward
    digitalWrite(ML1, LOW);
    digitalWrite(ML2, HIGH);

    // turn right motor backward
    digitalWrite(MR1, HIGH);
    digitalWrite(MR2, LOW);

    enable(true);
}

void turnLeft(void) {
    disable();
    
    int s = 40;

    // turn left motor backward
    digitalWrite(ML1, HIGH);
    digitalWrite(ML2, LOW);

    // turn right motor forward
    digitalWrite(MR1, LOW);
    digitalWrite(MR2, HIGH);

    enable(true);
}

void stop(void) { disable(); }

