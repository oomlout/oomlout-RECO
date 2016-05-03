int stripLength = 51;
int numStrips = 42;
//float mmPerStep = 0.2539/8; //mm per step (bigger shortwer)

float mmPerStep = 0.2539/8; //mm per step (bigger shortwer--)

// include the library code:
#include <LiquidCrystal.h>
#include <Servo.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

Servo leftBlade;
Servo rightBlade;

int rightBladePin = 13;
int leftBladePin = 12;

int rightBladeUp = 90;
int leftBladeUp = 90;

int rightBladeDown = 20;
int leftBladeDown = 160;

int stepPin = 11;
int dirPin = 10;

int buttonPin = 3;

int advancePin = 19;
int reversePin = 17;
int resetPin = 16;

boolean CW = LOW;
boolean CCW = HIGH;

int stepsPer = 400;

int pulseDelay = 300 ; //Pulse length in microseconds

int longDelay = 2000;
int shortDelay = 500;

//2 meters = 8.5 turns

int resCount = 0;

void setup(){
  Serial.begin(9600);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(buttonPin, INPUT);
  pinMode(advancePin, INPUT);
  pinMode(reversePin, INPUT);

  pinMode(rightBladePin, OUTPUT);
  pinMode(leftBladePin, OUTPUT);

  rightBlade.attach(rightBladePin); 
  leftBlade.attach(leftBladePin);

  bladesUp();

  Serial.begin(9600);
  digitalWrite(buttonPin, HIGH);
  digitalWrite(advancePin, HIGH);
  digitalWrite(reversePin, HIGH);
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Resistor Count");
  delay(2000);
}


boolean run = true;

void loop(){
  //  Serial.println(digitalRead(buttonPin));
  if(digitalRead(resetPin) == LOW){
//    resCount = 0;
  } 
  if(digitalRead(advancePin) == LOW){
    pulseDelay = longDelay;
    advanceMM(1);
    digitalWrite(dirPin, CCW);
    pulseDelay = shortDelay;
  } 
  if(digitalRead(reversePin) == LOW){
    pulseDelay = longDelay;
    advanceMM(1);
    digitalWrite(dirPin, CW);
    pulseDelay = shortDelay;
  }
//  if(digitalRead(buttonPin) == LOW){
  if(run == true){    
    if(resCount < numStrips)  {
      digitalWrite(dirPin, CCW);
      advanceResistors(stripLength);
      resCount++;
      delay(250);
      bladesDown();
      delay(1000);
      bladesUp();
      delay(1000);
      lcd.setCursor(0, 1);
      lcd.print(resCount); 
      Serial.print("RESISTOR STRIP # ");
      Serial.print(resCount);
      Serial.print("\tof\t");
      Serial.print(numStrips);
      Serial.println();
//      run = false;
    } 
  }
}

void bladesUp(){
  leftBlade.write(leftBladeUp);
  rightBlade.write(rightBladeUp);
}




void bladesDown(){
  leftBlade.write(leftBladeDown);
  rightBlade.write(rightBladeDown);
}

void advanceResistors(float resis){
  advanceMM(resis * 5.08); 
}

void advanceMM(float mmToGo){
  float advance = mmToGo / mmPerStep;
  rotate(advance);
}


void rotate(int steps){
  for(int i = 0; i < steps; i++){
    delayMicroseconds(pulseDelay/2);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(pulseDelay/2);
    digitalWrite(stepPin, LOW);
  } 
}

void rotateTimes(int times){
  for(int i = 0; i < times; i++){
    rotate(stepsPer);
  }
}

