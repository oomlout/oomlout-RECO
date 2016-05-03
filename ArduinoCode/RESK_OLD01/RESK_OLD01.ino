#include <EEPROM.h>

int stripLength = 5;
int numStrips = 50;
//float mmPerStep = 0.2539/8; //mm per step (bigger shortwer)

float mmPerStep = 0.2539/8; //mm per step (bigger shortwer--)

// include the library code:
#include <LiquidCrystal.h>
#include <Servo.h>

//LCD Declarations
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

//EEProm Declarations
int eepNumStrips = 1;
int eepStripLength = 2;


//Servo Declarations
Servo leftBlade;
Servo rightBlade;

int rightBladePin = 13;
int leftBladePin = 12;

int rightBladeUp = 90;
int leftBladeUp = 90;

int rightBladeDown = 20;
int leftBladeDown = 160;

//PIEZO DECLARATION
int piezoPin = 2;

//Button Declarations
int resPluPin = 18;
int resNegPin = 19;

int strPluPin = 16;
int strNegPin = 17;

int calPluPin = 14;
int calNegPin = 15;

int buttonPin = 3;

//Stepper Driver Declarations
int stepPin = 11;
int dirPin = 10;

boolean CW = LOW;
boolean CCW = HIGH;

int stepsPer = 400;

int pulseDelay = 300 ; //Pulse length in microseconds

int longDelay = 2000;
int shortDelay = 300;

int resCount = 0;

boolean runningVar = true;

void setup(){
  Serial.begin(9600);
//StepperDriver Pins
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
//ButtonPins  
  pinMode(buttonPin, INPUT);
  digitalWrite(buttonPin, HIGH);

  pinMode(resPluPin, INPUT);
  digitalWrite(resPluPin, HIGH);  
  pinMode(resNegPin, INPUT);
  digitalWrite(resNegPin, HIGH);  

  pinMode(strPluPin, INPUT);
  digitalWrite(strPluPin, HIGH);  
  pinMode(strNegPin, INPUT);
  digitalWrite(strNegPin, HIGH);  

  pinMode(calPluPin, INPUT);
  digitalWrite(calPluPin, HIGH);  
  pinMode(calNegPin, INPUT);
  digitalWrite(calNegPin, HIGH);  
//PIEZO setup
  pinMode(piezoPin, OUTPUT);

//Servo Setup
  pinMode(rightBladePin, OUTPUT);
  pinMode(leftBladePin, OUTPUT);
  rightBlade.attach(rightBladePin); 
  leftBlade.attach(leftBladePin);
  bladesUp();

//LCDSetup
  Serial.begin(9600);

  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.noCursor();
  lcd.print("    .:RECO:.");
  lcd.setCursor(0,1);
  lcd.print("Resistor Cutter");
  delay(2000);
  lcd.clear();
  stripLength = EEPROM.read(eepStripLength);
  numStrips = EEPROM.read(eepNumStrips);

  updateDisplay();
  if(digitalRead(buttonPin) == LOW){
    runningVar = false;    
  }
}


void loop(){
  updateDisplay();
  buttonScan();
  delay(100);
}

void buttonScan(){
  
  if(digitalRead(buttonPin) == LOW) {
    cutOnce();
  }

  if(digitalRead(buttonPin) == HIGH) {
    runningVar = true;
  }

  
  if(digitalRead(resPluPin) == LOW) {
    stripLength++;
  }
   
  if(digitalRead(resNegPin) == LOW) {
   stripLength--; 
  } 
  
  if(digitalRead(strPluPin) == LOW) {
    numStrips++;
  }
  
  if(digitalRead(strNegPin) == LOW) {
    numStrips--;
  }
  
 if(digitalRead(calPluPin) == LOW) {
  while(digitalRead(calPluPin) == LOW){
      pulseDelay = longDelay;
      digitalWrite(dirPin, CCW);
      rotate(1);
      pulseDelay = shortDelay;
  }
 }
  
 if(digitalRead(calNegPin) == LOW) {
  while(digitalRead(calNegPin) == LOW){
    pulseDelay = longDelay;
    digitalWrite(dirPin, CW);
    rotate(1);
    pulseDelay = shortDelay;
  }
 }
  
}

void updateDisplay(){
  lcd.setCursor(0, 0);
  lcd.print("len:");
  lcd.setCursor(4, 0);
  lcd.print("    ");
  lcd.setCursor(4, 0);
  lcd.print(stripLength);
  lcd.setCursor(8, 0);
  lcd.print("amt:");
  lcd.setCursor(12, 0);
  lcd.print("    ");
  lcd.setCursor(12, 0);
  lcd.print(numStrips);
  lcd.setCursor(0, 1);
  lcd.print("count: ");
  lcd.setCursor(7, 1);
  lcd.print("    ");
  lcd.setCursor(7, 1);
  lcd.print(resCount);
}

void cutOnce(){
     if(resCount < numStrips)  {
       if(runningVar) {
        digitalWrite(dirPin, CCW);
        resCount++;
         updateDisplay();
        advanceResistors(stripLength);
        delay(250);
        bladesDown();
        delay(1000);
        bladesUp();
        delay(1500);
        } 
     }
     if(resCount == numStrips)  {
         beep();
         runningVar = false;
         resCount = 0;
         EEPROM.write(eepStripLength,stripLength);
         EEPROM.write(eepNumStrips,numStrips);        
     }
}

void bladesUp(){
  leftBlade.write(leftBladeUp);
  rightBlade.write(rightBladeUp);
}

void beep(){
 int pause = 250;
 digitalWrite(piezoPin, HIGH) ;
 delay(pause);
  digitalWrite(piezoPin, LOW) ;
 delay(pause);
  digitalWrite(piezoPin, HIGH) ;
 delay(pause);
  digitalWrite(piezoPin, LOW) ;
 delay(pause);
   digitalWrite(piezoPin, HIGH) ;
 delay(pause);
  digitalWrite(piezoPin, LOW) ;
 delay(pause);
   digitalWrite(piezoPin, HIGH) ;
 delay(pause);
  digitalWrite(piezoPin, LOW) ;
 delay(pause);
   digitalWrite(piezoPin, HIGH) ;
 delay(pause);
  digitalWrite(piezoPin, LOW) ;
 delay(pause);
   digitalWrite(piezoPin, HIGH) ;
 delay(pause);
  digitalWrite(piezoPin, LOW) ;
 delay(pause);
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

