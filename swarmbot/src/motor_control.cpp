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

    pinMode(E1, OUTPUT);
    pinMode(E2, OUTPUT);
}

/* initialize to some speed */
static int speed = 1;

/* converts angular velocity to pwm value via some experimentally 
 * found constant
 */
static int speed_to_pwm(int s) {
    if (s < MIN_SPEED || s > MAX_SPEED) s = 3;
    return 200 * s; /* minimum is 0, max is 255 */
}

static void enable(int forward)  { 
    if (forward) {
        analogWrite(E1, speed_to_pwm(speed));
        analogWrite(E2, speed_to_pwm(speed));
    }
    else { /* backward */
        analogWrite(E1, speed_to_pwm(speed));
        analogWrite(E2, speed_to_pwm(speed));
    }
}

static void disable(void) { analogWrite(E1, 0);     analogWrite(E2, 0);     }

/* -------------------------- INTERFACE -------------------------- */

/* going to need an init function to initialize pins and speed */

void set_speed(int s) { speed = s;    }
int  get_speed(void)  { return speed; }

void forward(void) {
    disable();

    digitalWrite(ML2, LOW);
    digitalWrite(ML1, HIGH);

    digitalWrite(MR1, LOW);
    digitalWrite(MR2, HIGH);

    enable(true);
}

void backward(void) {
    disable();

    digitalWrite(ML2, HIGH);
    digitalWrite(ML1, LOW);

    digitalWrite(MR1, HIGH);
    digitalWrite(MR2, LOW);

    enable(false);
}

// turn both motors in opposite directions to pivot
void turn(int angle) {
    disable();
    
    /* these constants were found experimentally */
    int s = 35, turn_constant = 18;

    if (angle > 0) { // turn left
        // turn right motor forward
        digitalWrite(ML1, LOW);
        digitalWrite(ML2, HIGH);

        // turn left motor backward
        digitalWrite(MR1, LOW);
        digitalWrite(MR2, HIGH);
    } else { // turn right
        // turn right motor backward
        digitalWrite(ML1, HIGH);
        digitalWrite(ML2, LOW);

        // turn left motor forward
        digitalWrite(MR1, HIGH);
        digitalWrite(MR2, LOW);
    }

    analogWrite(E1, s);
    analogWrite(E2, s);

    angle = (angle > 0) ? angle : 0 - angle; // abs(angle)
    delay(angle * turn_constant);            // measured constant
    
    disable();
}

void turnLeft(void) {
    disable();
    
    int s = 40;

    // turn right motor forward
    digitalWrite(ML1, LOW);
    digitalWrite(ML2, HIGH);

    // turn left motor backward
    digitalWrite(MR1, LOW);
    digitalWrite(MR2, HIGH);

    analogWrite(E1, s);
    analogWrite(E2, s);
}

void turnRight(void) {
    disable();
    
    int s = 40;

    // turn right motor backward
    digitalWrite(ML1, HIGH);
    digitalWrite(ML2, LOW);

    // turn left motor forward
    digitalWrite(MR1, HIGH);
    digitalWrite(MR2, LOW);

    analogWrite(E1, s);
    analogWrite(E2, s);
}

void stop(void) { disable(); }

