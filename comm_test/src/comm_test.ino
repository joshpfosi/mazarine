void setup() {

  pinMode(13, OUTPUT); // Set this pin as output
  pinMode(5, OUTPUT);
  pinMode(2, OUTPUT);

  Serial.begin(9600); Serial1.begin(1200);

  TCCR3A = _BV(COM3A0) | _BV(COM3B0) | _BV(WGM30) | _BV(WGM31);

  // sets COM Output Mode to FastPWM with toggle of OC3A on compare match with OCR3A
  // also sets WGM to mode 15: FastPWM with top set by OCR3A
  TCCR3B = _BV(WGM32) | _BV(WGM33) |  _BV(CS31);

  // sets WGM as stated above; sets clock scaling to "divide by 8"
  OCR3A = 63;

  // above sets the counter value at which register resets to 0x0000;
  // generate 20kHz when OCR3A=50
  // Serial.println(TCCR3A, BIN);Serial.println(TCCR3B, BIN);
}



void loop() {
    //your code here
}

