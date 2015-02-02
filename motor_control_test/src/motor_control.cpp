/* motor_control.c
 *
 * Basic concept is that forward/backward/stop are stateless, and simply set
 * the hardware in motion, while turning is handled internally, preventing
 * any other bot functionality while it is being executed
 * 
 * The bot speed is held in a static global to avoid the overhead of structs
 */

#include "motor_control.h"

static int speed;

void forward(void) {
    /* stuff */
}

void backward(void) {
    /* stuff */
}

void turn(int angle) {
    /* turn bot */
    stop();
}

void stop(void) {
    /* code to stop bot (enables get 0) */
}

void set_speed(int s) {
    speed = s;
    /* code to actually change speed */
}

int get_speed(void) {
    return speed;
}
