#define INPIN   22
#define OUTPIN  12
#define CARRIER 5

#define TIMEOUT       1000
#define DATA_DELAY    416  //  1/(1.2*10^3)/2 = 416 aka 1.2 kHz
#define PROTOCOL_LEN  24
#define ONE_THRESHOLD 140
#define BIT_SIZE      25
#define MSG_LEN       8

const static bool receivingBot = false; // change for Arduino
static bool waiting            = false;
static bool receiving          = false;

void setup()
{
    pinMode(INPIN,  INPUT);
    pinMode(OUTPIN, OUTPUT);
    
    // use interrupts on the input comm pin to manage state of communication subsystem
    attachInterrupt(INPIN, setWaiting, RISING);
    attachInterrupt(INPIN, setReceiving, FALLING);

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
    1,1,0,0,1,1,1,0
};

unsigned recMsg[] = {
    0,0,0,0,0,0,0,0
};

unsigned hsMsg[] = {
    1,1,1,1,0,0,0,0
};

void setWaiting(void) {
  if (!waiting) waiting = true;
}

void setReceiving(void) {
  if (!receiving && waiting) receiving = true;
}

void loop() {
  if (receivingBot) {
    // execute code to anticipate and respond to messages
    while (!waiting && !receiving) {} // idle until we see a handshake
    while (waiting &&  !receiving) {} // do nothing until falling edge
    
    // if here, we've seen a waiting falling edge AND a receiving rising edge
    receive(recMsg, MSG_LEN);
    
    // testing only
    for (int i = 0; i < MSG_LEN; ++i) {
      Serial.print(recMsg[i]);
    }
    Serial.print("\n");
  }
  else {
    // execute code to transmit a regular message
    transmit(msg, MSG_LEN);
    //delay(3000);
  }
}

/* transmits num 1s by flashing LED */
void transmit(unsigned msg[], unsigned len) {
  sendBits(hsMsg, MSG_LEN); // sending handshake so other bot knows message is coming
  digitalWrite(OUTPIN, LOW); // ensure low afterwards
  delayMicroseconds(500);
  sendBits(msg, len);
  digitalWrite(OUTPIN, LOW); // ensure low afterwards
}

void sendBits(unsigned bits[], unsigned len) {
    for (unsigned i = 0; i < len; ++i) {
      for (unsigned j = 0; j < BIT_SIZE; ++j) {
        digitalWrite(OUTPIN, (bits[i]) ? HIGH : LOW);
      }
      delayMicroseconds(2 * DATA_DELAY);      
    }
}

void receive(unsigned bits[], unsigned len) {
  waiting = false; // we are receiving, not waiting
    for (unsigned i = 0; i < MSG_LEN; ++i) {
      unsigned sum = 0;
      for (unsigned j = 0; j < BIT_SIZE; ++j) {
        // read all 25 bits, recording sum
        sum += digitalRead(INPUT);
        delayMicroseconds(2 * DATA_DELAY);
      }
      // average sum of 25 bits, adding value to bit[i]
      bits[i] = ((float)sum / BIT_SIZE > 0.5);
    }
    receiving = false; // we are done receiving
}
