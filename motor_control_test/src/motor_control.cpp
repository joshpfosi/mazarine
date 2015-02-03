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

/* initialize to some speed */
static int speed = 5;

/* going to need an init function to initialize pins and speed */

static void enable(void) {
    analogWrite(E1, 30); analogWrite(E2, 30);
}

static void disable(void) {
    analogWrite(E1, 0); analogWrite(E2, 0);
}

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

void set_speed(int s) {
    speed = s;
    /* code to actually change speed */
}

int get_speed(void) {
    return speed;
}

int speed_to_pwm(int speed) {
    /* converts angular velocity to pwm value
     * via some experimentally found constant
     */
}

