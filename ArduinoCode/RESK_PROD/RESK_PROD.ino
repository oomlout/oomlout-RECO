#include <EEPROM.h>



int stripLength = 5;
int numStrips = 50;
//float mmPerStep = 0.2539/8; //mm per step (bigger shortwer)

float mmPerStep = 0.2539/4; //mm per step (bigger shortwer--)

// include the library code:
#include <LiquidCrystal.h>
#include <Servo.h>

//BUTTON DECLARATIONS
int buttonPin = 0; //analog pin the button is attached to


//Original Pin numbers
//int resPlusButt = 4;
//int resNegButt = 5;  
//int stripPlusButt = 2;
//int stripNegButt = 3;  
//int calPlusButt = 0;
//int calNegButt = 1;  

//New Pin numbers
int resPlusButt = 0;
int resNegButt = 1;  
int stripPlusButt = 2;
int stripNegButt = 3;  
int calPlusButt = 4;
int calNegButt = 5;  





//Sensor declarations
int resSensorPin = 1;
int resPresent = 500;
int resNotThere = 880;
int resMiddleValue = (resPresent + resNotThere) / 2;

//LCD Declarations
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(9, 8, 7, 6, 5, 4);

//EEProm Declarations
int eepNumStrips = 1;
int eepStripLength = 2;
int eepCount = 3;

//Servo Declarations
Servo leftBlade;
Servo rightBlade;

int rightBladePin = 13;
int leftBladePin = 12;

int rightBladeUp = 90+60;
int leftBladeUp = 90-60;

int rightBladeDown = 0+10;
int leftBladeDown = 180-10;

//PIEZO DECLARATION
int piezoPin = 2;

//Button Declarations
//int resPluPin = 18;
//int resNegPin = 19;

//int strPluPin = 16;
//int strNegPin = 17;

//int calPluPin = 14;
//int calNegPin = 15;

int runningPin = 3;

//Stepper Driver Declarations
int stepPin = 11;
int dirPin = 10;

boolean CW = LOW;
boolean CCW = HIGH;

int stepsPer = 400;

//int pulseDelay = 300 ; //Pulse length in microseconds
int pulseDelay = 1500 ; //Pulse length in microseconds

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
  pinMode(runningPin, INPUT);
  digitalWrite(runningPin, HIGH);

//Un comment if resetting  
  loadDefaultValuesIntoEE();
  

//  pinMode(resPluPin, INPUT);
//  digitalWrite(resPluPin, HIGH);  
//  pinMode(resNegPin, INPUT);
//  digitalWrite(resNegPin, HIGH);  

//  pinMode(strPluPin, INPUT);
//  digitalWrite(strPluPin, HIGH);  
//  pinMode(strNegPin, INPUT);
//  digitalWrite(strNegPin, HIGH);  

//  pinMode(calPluPin, INPUT);
//  digitalWrite(calPluPin, HIGH);  
//  pinMode(calNegPin, INPUT);
//  digitalWrite(calNegPin, HIGH);  
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
  resCount = EEPROM.read(eepCount);
  updateDisplay();
  if(digitalRead(runningPin) == LOW){
    runningVar = false;    
  }

}


void loop(){
  updateDisplay();
  buttonScan();
  checkRunning();
  delay(100);
}

void checkRunning(){
  Serial.println("CheckRunning:");
  Serial.println(digitalRead(runningPin) == LOW);
  if(digitalRead(runningPin) == HIGH) {
    runningVar = true;
  }
  
  if(digitalRead(runningPin) == LOW) {
    cutOnce();
  }
 
}

void buttonScan(){
  
  int checking = checkButtons();

  if((checking == resPlusButt)) {stripLength++;}
   
  if((checking == resNegButt)) {stripLength--;} 
  
  if((checking == stripPlusButt)) {numStrips++;}
  
  if((checking == stripNegButt)) {numStrips--;}
  
  if((checking == calPlusButt)) {
    while((checking == calPlusButt)){
      checking = checkButtons();
      pulseDelay = longDelay;
      digitalWrite(dirPin, CCW);
      rotate(1);
      pulseDelay = shortDelay;
  }
 }
 
 if((checking == calNegButt)) {
    while((checking == calNegButt)){
    checking = checkButtons();
    pulseDelay = longDelay;
    rotate(-1);
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
  lcd.print("  ");
  lcd.print(checkResistor());
}

void cutOnce(){
  Serial.println("CutOnce:");
     if(resCount < numStrips)  {
       Serial.print("  Running Var:");
       Serial.println(runningVar);
       if(runningVar) {
        updateDisplay();
        advanceResistors(stripLength);
        resCount++;
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
         loadCurrentValuesIntoEE();
     }
}

void bladesUp(){
  leftBlade.write(leftBladeUp);
  rightBlade.write(rightBladeUp);
}

void longBeep(){
   int pause = 3000;
 digitalWrite(piezoPin, HIGH) ;
 delay(pause);
  digitalWrite(piezoPin, LOW) ;
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

      advanceMM((resis) * 5.08); 
//    advanceMM((resis-1) * 5.08); 
  
//  //ADVANCE ONE LESS THEN DEAD RECKON TO A GOOD CUT SPOT
//    int result = advanceOneResistor(); 
//    if(result != 1){
//      waitForErrorClear();
//    }
  

//  advanceMM(0);
}

void advanceMM(float mmToGo){
  float advance = mmToGo / mmPerStep;
  rotate(advance);
}

void waitForErrorClear(){
  lcd.setCursor(0,1);
  lcd.print("Advance Error"); 
  longBeep();
  
  int checking = checkButtons();
  
  while(checking != 5){
   checking = checkButtons();
     
    if((checking == calPlusButt)) {
      while((checking == calPlusButt)){
        checking = checkButtons();
        pulseDelay = longDelay;
        digitalWrite(dirPin, CCW);
        rotate(1);
        pulseDelay = shortDelay;
    }

   }
    if((checking == calNegButt)) {
      while((checking == calNegButt)){
      checking = checkButtons();
      pulseDelay = longDelay;
      rotate(-1);
      pulseDelay = shortDelay;
    }
   }
   updateDisplay();
   lcd.setCursor(0,1);
   lcd.print("Advance Error"); 
  }
  

  
  
}


void rotate(int steps){
  if(steps > 0){
    digitalWrite(dirPin, CCW);
  }else{
    digitalWrite(dirPin, CW);
  }
  steps = abs(steps);
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

//returns 1 if succesful -1 if not
int advanceOneResistor(){
  Serial.println("advance one resistor Start:");
  int returnValue = -1;
 
 //number of steps before suggesting there's an error
 int maxAdvance = 400; //(2.54 is about 80 steps)
 int initialAdvance = 75; 
 int stepsPer = 120; //average number of steps (not used)
 
 int checking = checkResistor();
 int timesThrough = 0;
 //if resistor present advance until clear)
 if(checking == 1){
    while(checking == 1){
      rotate(1);
      checking = checkResistor();
      if(checking == 1){
        returnValue = 1;
      }
      timesThrough++;
      if(timesThrough > maxAdvance){
        checking = 0;
      }
   }  
 }
 
 //initialAdvance (done in a for loop to make the speed the same as other advance)
 for(int i = 0; i < initialAdvance; i++){
   rotate(1);
 }   
 timesThrough = initialAdvance;
 checking = checkResistor();
 while(checking == 0){
    rotate(1);
    checking = checkResistor();
    if(checking == 1){
      returnValue = 1;
    }
    timesThrough++;
    if(timesThrough > maxAdvance){
      checking = 1;
    }
 }
 return returnValue;
}


int checkButtons(){
 int returnValue = -1;
 int buttValue[] = {1023, 462, 289, 202,144,90};
 int bufferValue = 40;
 int numButtons = 6;
 int reading = analogRead(buttonPin);
 for(int i = 0; i < numButtons; i++){
  if(abs(reading - buttValue[i]) < bufferValue){
    returnValue = i;
  }
 }
 return returnValue;
}


//returns 1 if a resistor is present 0 if not
int checkResistor(){
  int returnValue = 0;
  if(analogRead(resSensorPin) < resMiddleValue){
   returnValue = 1; 
  }
  return returnValue;
}
  
void loadCurrentValuesIntoEE() {
  EEPROM.write(eepStripLength,stripLength);
  EEPROM.write(eepNumStrips,numStrips);       
  EEPROM.write(eepCount,0);       
}

void loadDefaultValuesIntoEE() {
  EEPROM.write(eepStripLength,5);
  EEPROM.write(eepNumStrips,100);       
  EEPROM.write(eepCount,0);       
}


