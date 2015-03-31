#define INPIN   22
#define OUTPIN  12
#define CARRIER 5

#define TIMEOUT       1000
#define DATA_DELAY    416  //  1/(1.2*10^3)/2 = 416 aka 1.2 kHz
#define PROTOCOL_LEN  24
#define ONE_THRESHOLD 140

void setup()
{
    pinMode(INPIN,  INPUT);
    pinMode(OUTPIN, OUTPUT);

    pinMode(CARRIER, OUTPUT); 
    Serial.begin(9600);
    TCCR3A = _BV(COM3A0) | _BV(COM3B0) | _BV(WGM30) | _BV(WGM31);
    // sets COM Output Mode to FastPWM with toggle of OC3A on compare match with OCR3A
    // also sets WGM to mode 15: FastPWM with top set by OCR3A

    TCCR3B = _BV(WGM32) | _BV(WGM33) |  _BV(CS31);
    // sets WGM as stated above; sets clock scaling to "divide by 8"

    OCR3A = 55; // Used to be 63
    // above sets the counter value at which register resets to 0x0000;
    // generate 20kHz when OCR3A=50
    // Serial.println(TCCR3A, BIN);Serial.println(TCCR3B, BIN);

}

unsigned msg[] = { 
    1,1,0,0,1,1,1,0,
    1,1,0,0,1,1,1,0,
    1,1,0,0,1,1,1,0,
};

unsigned recMsg[] = {
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,
};

void loop() {
    unsigned len = 24;
    receive(recMsg, len);

    Serial.print("recMsg=");
    for (int i = 0; i < len; ++i) {
        Serial.print(recMsg[i]);
    }
    Serial.print("\n");
    delay(1000);
    //transmit(msg, len);
}

/* transmits num 1s by flashing LED */
void transmit(unsigned bits[], unsigned len) {
    for (unsigned i = 0; i < len; ++i) {
        digitalWrite(OUTPIN, (bits[i]) ? HIGH : LOW);
        delayMicroseconds(2 * DATA_DELAY);
    }

    digitalWrite(OUTPIN, LOW); // ensure low afterwards
}

void receive(unsigned bits[], unsigned len) {
    for (unsigned i = 0; i < len; ++i) {
        bits[i] = (analogRead(INPUT) > ONE_THRESHOLD);
        delayMicroseconds(2 * DATA_DELAY);
    }
}

