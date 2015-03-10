////output LEDs
//int front_left_led = 4;
//int front_right_led = 5;
//int side_left_led = 6;
//int side_right_led= 7;
//int back_bumper_led = 8;
//collision flags
volatile boolean front_left_on = 0;
volatile boolean front_right_on = 0;
volatile boolean side_left_on = 0;
volatile boolean side_right_on = 0;
volatile boolean back_bumper_on = 0;

//input pins
int front_left = A0; //changed
int front_right = A1;
int side_left = A4;
int side_right= A3;
int back_bumper = A2;
int switchThreshold = 250;

//delays and debounce timers
//long currentTime = 0;
//int flashDelay = 1000;
long lastDebounceTime0 = 0;
long lastDebounceTime1 = 0;
long lastDebounceTime2 = 0;
long lastDebounceTime3 = 0;
long lastDebounceTime4 = 0;
int debounceDelay = 100;



void setup()
{ 

 //INTERRUPT SETUP
  attachInterrupt(0, collision_ISR, RISING);

  //INPUT PINS SETUP
  pinMode(front_left, INPUT);
  pinMode(front_right, INPUT);
  pinMode(side_left, INPUT);
  pinMode(side_right, INPUT);
  pinMode(back_bumper, INPUT);
  

  Serial.begin(9600);
}
 
void loop()
{
   //check if any flags are set and if so, light the appropriate LED
   if (front_left_on == true)
   {
     //handle front left collision
     Serial.println("front left");
   }
   if (front_right_on == true)
   {
     //handle front right collision
     Serial.println("front right");
   }
   if (side_left_on == true)
   {
     //handle side left collision
     Serial.println("side left");
      
   }
   if (side_right_on == true)
   {
     //handle side right collision
      Serial.println("side right");
   }
   if (back_bumper_on == true)
   {
     //handle back bumper collision
     Serial.println("back");
   }
   //reset flags
   front_left_on = false;
   front_right_on = false;
   side_left_on = false;
   side_right_on = false;
   back_bumper_on = false;


   
}
 
 
void collision_ISR()
{
  //Serial.println("ISR");
  // set collision flags
  if (millis() - lastDebounceTime0 > debounceDelay) //only enter ISR if debounceDelay ms has passed since last ISR
  {
    lastDebounceTime0 = millis();
    if (analogRead(front_left) > switchThreshold)
    {
      front_left_on = true;
    }
  }
  
  if (millis() - lastDebounceTime1 > debounceDelay) 
  {
    lastDebounceTime1 = millis();
    if (analogRead(front_right) > switchThreshold)
    {
      front_right_on = true;
    }
  }
  
  if (millis() - lastDebounceTime2 > debounceDelay) 
  {
    lastDebounceTime2 = millis();
    if (analogRead(side_left) > switchThreshold)
    {
      side_left_on = true;
    }
  }
  
  if (millis() - lastDebounceTime3 > debounceDelay)
  {
    lastDebounceTime3 = millis();
    if (analogRead(side_right) > switchThreshold)
    {
      side_right_on = true;
    }
  }
  
  if (millis() - lastDebounceTime4 > debounceDelay)
  {
    lastDebounceTime4 = millis();
    if (analogRead(back_bumper) > switchThreshold)
    {
      back_bumper_on = true;
    }
  } 
  
}
