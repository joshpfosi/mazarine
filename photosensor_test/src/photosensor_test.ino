/* photosensor_test.ino
 * Flashes LEDs to test photosensor and color detection
 */

#define RED_LED      12
#define BLUE_LED     13
#define RED_IND      5
#define BLUE_IND     6
#define YELLOW_IND     7
#define PHOTO_SENSOR A0

void setup() {
    pinMode(RED_LED,      OUTPUT);
    pinMode(BLUE_LED,     OUTPUT);
    pinMode(PHOTO_SENSOR, INPUT);
    pinMode(RED_IND,      OUTPUT);
    pinMode(BLUE_IND,     OUTPUT);
    pinMode(YELLOW_IND,     OUTPUT);
    
    Serial.begin(9600);
}

void loop() { 
    int sensor, red, blue, both, isRed, isBlue, isYellow;
    char msg[100];
    
    //
    // Read sensor value for red, blue and both
    //
    digitalWrite(RED_LED,  HIGH);
    digitalWrite(BLUE_LED, LOW);
    delay(30);
    red = analogRead(PHOTO_SENSOR);

    digitalWrite(BLUE_LED, HIGH);
    digitalWrite(RED_LED,  LOW);
    delay(30);
    blue = analogRead(PHOTO_SENSOR);

    digitalWrite(BLUE_LED, HIGH);
    digitalWrite(RED_LED,  HIGH);
    delay(30);
    both = analogRead(PHOTO_SENSOR);

    // turn them off
    digitalWrite(BLUE_LED, LOW);
    digitalWrite(RED_LED,  LOW);

    isRed  = (500 < red && red < 600);
    isBlue = (800 < red && red < 930);
    isYellow = (red < 300);

    digitalWrite(RED_IND,    LOW);
    digitalWrite(BLUE_IND,   LOW);
    digitalWrite(YELLOW_IND, LOW);
    
    delay(10);

    if (isRed) {
      digitalWrite(RED_IND,    HIGH);
      digitalWrite(BLUE_IND,   LOW);
      digitalWrite(YELLOW_IND, LOW);
      Serial.print("I'm red");
    }

    if (isBlue) {
      digitalWrite(RED_IND,    LOW);
      digitalWrite(BLUE_IND,   HIGH);
      digitalWrite(YELLOW_IND, LOW);
      Serial.print("I'm blue");
    }

    if (isYellow) {
      digitalWrite(RED_IND,    LOW);
      digitalWrite(BLUE_IND,   LOW);
      digitalWrite(YELLOW_IND, HIGH);
      Serial.print("I'm yellow");
    }

    sprintf(msg, "Photo sensor values: Blue = %d, Red = %d, Both = %d\n", blue, red, both);
    Serial.print(msg);
}

