/* motor_control.ino
 * Implementation of motor control functions
 */

#define FORWARD  A2
#define BACKWARD A3
#define ON       255
#define OFF      0

void setup() {
    /* set up pins for OUTPUT */
    pinMode(FORWARD,  OUTPUT);
    pinMode(BACKWARD, OUTPUT);
}

int check = 0;

void loop() {
    check = !check;
    if (check) {
        analogWrite(FORWARD,  ON);
        analogWrite(BACKWARD, OFF);
    }
    else {
        analogWrite(BACKWARD, ON);
        analogWrite(FORWARD,  OFF);
    }
    delay(3000); /* run for 3 seconds, then switch */
}
