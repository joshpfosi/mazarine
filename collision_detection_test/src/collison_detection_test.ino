#define INT0 0 // pin 2
#define INT1 1 // pin 3
#define INT2 2 // pin 21
#define INT3 3 // pin 20
#define INT4 4 // pin 19
#define INT5 5 // pin 18

#define LED0 38
#define LED1 39
#define LED2 40
#define LED3 41
#define LED4 42
#define LED5 43

void setup() {
    pinMode(LED0, OUTPUT);
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    pinMode(LED3, OUTPUT);
    pinMode(LED4, OUTPUT);
    pinMode(LED5, OUTPUT);

    attachInterrupt(INT0, front,      FALLING);
    attachInterrupt(INT1, back,       FALLING);
    attachInterrupt(INT2, left,       FALLING);
    attachInterrupt(INT3, right,      FALLING);
    attachInterrupt(INT4, frontLeft,  FALLING);
    attachInterrupt(INT5, frontRight, FALLING);
}

volatile int state[6] = { HIGH, HIGH, HIGH, HIGH, HIGH, HIGH };

void loop() {
    digitalWrite(LED0, state[0]);
    digitalWrite(LED1, state[1]);
    digitalWrite(LED2, state[2]);
    digitalWrite(LED3, state[3]);
    digitalWrite(LED4, state[4]);
    digitalWrite(LED5, state[5]);
}

void front(void) {
    state[0] = !state[0];
    // stop
    // reverse for 1 sec
    // turn right 30 deg
}

void back(void) {
    state[1] = !state[1];
    // stop
    // straight for 0.5 sec
}

void left(void) {
    state[2] = !state[2];
    // stop
    // go straight for 0.5 sec
    // Left again
}

void right(void) {
    state[3] = !state[3];
    // stop
    // go straight for 0.5 sec
    // Right again
}

void frontLeft(void) {
    state[4] = !state[4];
    // stop
    // reverse for 1 sec
    // turn right 45 deg
}

void frontRight(void) {
    state[5] = !state[5];
    // stop
    // reverse for 1 sec
    // turn left 45 deg
}

