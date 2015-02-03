/* motor_control_test.ino
 * Calls each motor_control function to test them
 */

/* motor_control.c
 *
 * Basic concept is that forward/backward/stop are stateless, and simply set
 * the hardware in motion, while turning is handled internally, preventing
 * any other bot functionality while it is being executed
 * 
 * The bot speed is held in a static global to avoid the overhead of structs
 */

#include "motor_control.h"

/* initialize to some speed */
static int speed = 5,
           in1 = 24, in2 = 25, in3 = 30, in4 = 31,
           e1 = 2, e2 = 3;

/* going to need an init function to initialize pins and speed */

static void enable(void) {
    analogWrite(e1, 30); analogWrite(e2, 30);
}

static void disable(void) {
    analogWrite(e1, 0); analogWrite(e2, 0);
}

void forward(void) {
    disable();

    digitalWrite(in2, LOW);
    digitalWrite(in1, HIGH);

    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);

    enable();
}

void backward(void) {
    disable();

    digitalWrite(in2, HIGH);
    digitalWrite(in1, LOW);

    digitalWrite(in3, HIGH);
    digitalWrite(in4, LOW);

    enable();
}

void turn(int angle) {
    /* turn bot */
    stop();
    if (angle > 0) { /* turn left */
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
        analogWrite(e2, 100); /* MAKE SPEED A THING*/
    } else {
        digitalWrite(in2, LOW);
        digitalWrite(in1, HIGH);
        analogWrite(e2, 100); /* HERE TOO*/
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

void setup() {
    /* set up pins for OUTPUT */
    pinMode(30, OUTPUT);
    pinMode(31, OUTPUT);
    pinMode(24, OUTPUT);
    pinMode(25, OUTPUT);
    pinMode(2,  OUTPUT);
    pinMode(3,  OUTPUT);
}

void loop() {
    /* call all the functions */
    delay(3000);
    forward();
    delay(3000);
    stop();
    delay(1000);
    backward();
    //delay(3000);
    //stop();
    //delay(1000);
}
