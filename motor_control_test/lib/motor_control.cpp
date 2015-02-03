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
           in1 = 22, in2 = 23, in3 = 24, in4 = 25,
           e1 = 2, e2 = 3;

/* going to need an init function to initialize pins and speed */
 

void forward(void) {
    /* stuff */
    analogWrite(e1, 255);
    analogWrite(e1, 255);
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, HIGH);
}

void backward(void) {
    /* stuff */
}

void turn(int angle) {
    /* turn bot */
    stop();
}

void stop(void) {
    analogWrite(e1, 0);
    analogWrite(e2, 0);
}

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
