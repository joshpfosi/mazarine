/* motor_control_test.ino
 * Calls each motor_control function to test them
 */

#include "motor_control.h"

void setup() {
    /* set up pins for OUTPUT */
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(E1,  OUTPUT);
    pinMode(E2,  OUTPUT);
}

void loop() {
    /* call all the functions */
    set_speed(5);
    delay(300);
    forward();
    delay(300);
    stop();
    delay(1000);
    backward();
    delay(300);
    set_speed(3);
    delay(3000);
    //set_speed(2);
    //delay(3000);
    //forward();
    //delay(3000);
    //stop();
    //delay(1000);
    //backward();
    //delay(3000);

    //stop();
    //turn(90);
    //delay(500);
    //turn(-90);
    //delay(500);
    //turn(180);
}
