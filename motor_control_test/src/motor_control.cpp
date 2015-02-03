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

/* initialize to some speed */
static int speed = 3;

/* converts angular velocity to pwm value via some experimentally 
 * found constant
 */
static int speed_to_pwm(int s) {
    if (s < MIN_SPEED || s > MAX_SPEED) s = 3;
    return 51 * s; /* minimum is 0, max is 255 */
}

static void enable(void)  { analogWrite(E1, speed); analogWrite(E2, speed); }
static void disable(void) { analogWrite(E1, 0);     analogWrite(E2, 0);     }

/* -------------------------- INTERFACE -------------------------- */

/* going to need an init function to initialize pins and speed */

void set_speed(int s) { speed = s;    }
int  get_speed(void)  { return speed; }

void forward(void) {
    disable();

    digitalWrite(IN2, LOW);
    digitalWrite(IN1, HIGH);

    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);

    enable();
}

void backward(void) {
    disable();

    digitalWrite(IN2, HIGH);
    digitalWrite(IN1, LOW);

    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);

    enable();
}

void turn(int angle) {
    /* turn bot */
    stop();
    if (angle > 0) { /* turn left */
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        analogWrite(E2, 100); /* MAKE SPEED A THING*/
    } else {
        digitalWrite(IN2, LOW);
        digitalWrite(IN1, HIGH);
        analogWrite(E2, 100); /* HERE TOO*/
    }
    delay(30 /* ACTUAL MATH HERE */);
    
    /*stop or forward here??*/
    stop();
}

void stop(void) { disable(); }

