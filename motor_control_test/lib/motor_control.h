/* motor_control.h
 * This file exposes the motor control interface to implement motion control
 */

/* move the bot forward until another call is made */
void forward(void);

/* move the bot backward until another call is made */
void backward(void);

/* turn the bot angle degrees, and stops */
void turn(int angle);

/* stops the bot */
void stop(void);

/* sets/gets angular speed of bot tire */
void set_speed(int s);
int  get_speed(void);
