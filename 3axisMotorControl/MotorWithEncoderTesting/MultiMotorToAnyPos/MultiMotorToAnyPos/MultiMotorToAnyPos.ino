//----------------------------------------split it between two----------------------
//turns turret motor to assigned position
//turns reach arm to assigned position


#include "Arduino.h"
#include <digitalWriteFast.h>  // library for high performance reads and writes by jrraines

// Quadrature encoders
// Left encoder
#define c_LeftEncoderInterruptA 0
#define c_LeftEncoderInterruptB 1
#define c_LeftEncoderPinA 2
#define c_LeftEncoderPinB 3
#define LeftEncoderIsReversed

volatile bool _LeftEncoderASet;
volatile bool _LeftEncoderBSet;
volatile bool _LeftEncoderAPrev;
volatile bool _LeftEncoderBPrev;
volatile long _LeftEncoderTicks = 0;

const int buttonPin = 7;     // the number of the pushbutton limit pin for reach armint buttonState;         // variable for reading the pushbutton status
int buttonState;         // variable for reading the pushbutton status


const int laserPin = 6; //pin for IR breakbeam
int laserState; // variable to read IR breakbeam laser state

String inputHold; // used when we need to store the readString and trim it, in order to use it in comparisons

String readString;

#include <Servo.h>
#include <Wire.h>
#include <I2CEncoder.h>
float turretPos;
float reachPos;
int reader = 0;

Servo turretServo; // create servo object to control the Turret servo
Servo reachServo; // create servo object to control the Turret servo
//
// Create an I2CEncoder for use
I2CEncoder encoder1;//turret
I2CEncoder encoder2;//reach

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);

  // Initialize the encoder for a 269 motor
  encoder1.init(MOTOR_393_TORQUE_ROTATIONS, MOTOR_393_TIME_DELTA);
  encoder2.init(MOTOR_393_TORQUE_ROTATIONS, MOTOR_393_TIME_DELTA);

  turretServo.attach(9);                      // Attach left signal to pin 13
  reachServo.attach(10);
  pinMode(buttonPin, INPUT_PULLUP);


    // Quadrature encoders
  // Left encoder
  pinMode(c_LeftEncoderPinA, INPUT);      // sets pin A as input
  digitalWrite(c_LeftEncoderPinA, LOW);  // turn on pullup resistors
  pinMode(c_LeftEncoderPinB, INPUT);      // sets pin B as input
  digitalWrite(c_LeftEncoderPinB, LOW);  // turn on pullup resistors
  attachInterrupt(c_LeftEncoderInterruptA, HandleLeftMotorInterruptA, CHANGE);
  attachInterrupt(c_LeftEncoderInterruptB, HandleLeftMotorInterruptB, CHANGE);
}

void loop() {

 while (Serial.available()) {
    char c = Serial.read();  //gets one byte from serial buffer
    readString += c; //makes the string readString
    delay(2);  //slow looping to allow buffer to fill with next character
  }

  if (readString.length() >0) {
    Serial.println(readString);
    inputHold = readString;
    inputHold.trim();
    if(inputHold.equalsIgnoreCase("reset")){//if we type in reset, then go to original position
      returnToBase();
      //reachReturnToBase();
//        turretReturnToBase();
      Serial.println("returned");

    }

    else if(inputHold.equalsIgnoreCase("stats")){
      printStats();
      }
    
    else{
    int spaceIndex = readString.indexOf(" ");
    int turretNum = readString.substring(0,spaceIndex).toInt();  //first number of string
    int reachNum = readString.substring(spaceIndex+1,-1).toInt();  //second number of string
    
    wholeArmGoTo(turretNum,reachNum);
    //reachGoTo(reachNum);
    //turretGoTo(turretNum);


    }
  readString=""; //empty for next input
  } 
}






void wholeArmGoTo(int x,int y){
  //maybe add code to return to base pos
  turretGoTo(x);
  reachGoTo(y);
  
  }

void returnToBase(){
  turretReturnToBase();
  reachReturnToBase();
  
  }



void turretReturnToBase(){
  turretPos = _LeftEncoderTicks;//variable for position of turret
  Serial.println(turretPos);
  if (0 > turretPos) { //if new position is to right of current position
    while (0 > turretPos) {
      turretClockwise();//turn clockwise towards new position
      turretPos = _LeftEncoderTicks;//variable for position of turret
      printStats();//print out current speed and position
    }
    Serial.println("done first turn");
    turretServo.writeMicroseconds(1500);//stop
    delay(1000);//give it time for motor to overturn
    turretPos = _LeftEncoderTicks;//variable for position of turret
    while (0 < turretPos) {//correct for overturn slowly
      turretCounterClockwise();// turn in counterClockwise position towards new position
      turretPos = _LeftEncoderTicks;//variable for position of turret
      printStats();// print out current speed and position
    }
    
  }
  
  else if (0 < turretPos) { //if new position is to left of current position
    while (0 < turretPos) {
      turretCounterClockwise();// turn in counterClockwise position towards new position
      turretPos = _LeftEncoderTicks;//variable for position of turret
      printStats();// print out current speed and position
    }
    Serial.println("done first turn");
    turretServo.writeMicroseconds(1500);//stop
    delay(1000); //give it time for motor to overturn
    turretPos = _LeftEncoderTicks;//variable for position of turret
    while (0 > turretPos) {//correct for overturn 
      turretClockwise();//turn clockwise towards new position
      turretPos = _LeftEncoderTicks;//variable for position of turret
      printStats();//print out current speed and position
    }
    
  }
  turretServo.writeMicroseconds(1500);//stop
}










void reachReturnToBase(){//will make it go to base position based off of sensors
  buttonState = digitalRead(buttonPin);
  while (buttonState == HIGH){
    reachBackward();
    buttonState = digitalRead(buttonPin);
    }
  reachServo.writeMicroseconds(1500);//stop
  encoder2.zero();
  }


void turretGoTo(int newPos) {//full 360 degrees is 11ticks
  turretPos = _LeftEncoderTicks;//variable for position of turret
  Serial.println(turretPos);
  if (newPos > turretPos) { //if new position is to right of current position
    newPos--;//adjusting to account for overturn
    while (newPos > turretPos) {
      turretClockwise();//turn clockwise towards new position
      turretPos = _LeftEncoderTicks;//variable for position of turret
      printStats();//print out current speed and position
    }
  }
  else if (newPos < turretPos) { //if new position is to left of current position
    newPos++;//adjusting to account for overturn
    while (newPos < turretPos) {
      turretCounterClockwise();// turn in counterClockwise position towards new position
      turretPos = _LeftEncoderTicks;//variable for position of turret
      printStats();// print out current speed and position
    }
  }
  turretServo.writeMicroseconds(1500);//stop
}


void reachGoTo(int newPos) {//full extension is approx 2.5 ticks, will probably have to reduce gear ratio
  reachPos = encoder2.getPosition();//variable for position of reach arm
  if (newPos > reachPos) { //if new position is ahead of current position
    while (newPos > reachPos) {
      reachPos = encoder2.getPosition();//variable for position of reach arm
      reachForward();//move forward towards new position
      printStats();//print out current speed and position
    }
  }
  else if (newPos < reachPos) { //if new position is to left of current position
    while (newPos < reachPos) {
      reachPos = encoder2.getPosition();//variable for position of reach arm
      reachBackward();// move in backward towards new position
      printStats();// print out current speed and position
    }
  }
  reachServo.writeMicroseconds(1500);//stop
}



void printStats() {
  Serial.print(_LeftEncoderTicks); // Get how fast it's rotating
  Serial.print("     ");
  Serial.print(encoder2.getSpeed()); // Get how fast it's rotating
  Serial.print(", ");
  Serial.println(encoder2.getPosition()); // Get it's current position
}


//void turretCounterClockwiseSlow() { //move motor counterClockwise
//  turretServo.writeMicroseconds(1585);         // 1.7 ms -> counterclockwise
//}
//
//void turretClockwiseSlow() { //move motor counterClockwise
//  turretServo.writeMicroseconds(1415);         // 1.3 ms -> clockwise
//}





void turretCounterClockwise() { //move motor counterClockwise
  turretServo.writeMicroseconds(1600);         // 1.7 ms -> counterclockwise
}

void turretClockwise() { //move motor counterClockwise
  turretServo.writeMicroseconds(1400);         // 1.3 ms -> clockwise
}
void reachForward() { //move motor counterClockwise
  reachServo.writeMicroseconds(1700);         // 1.7 ms -> forward
}

void reachBackward() { //move motor counterClockwise
  reachServo.writeMicroseconds(1300);         // 1.7 ms -> backward
}




// Interrupt service routines for the right motor's quadrature encoder
void HandleLeftMotorInterruptB() {
  // Test transition;
  _LeftEncoderBSet = digitalReadFast(c_LeftEncoderPinB);
  _LeftEncoderASet = digitalReadFast(c_LeftEncoderPinA);

  _LeftEncoderTicks += ParseEncoder();

  _LeftEncoderAPrev = _LeftEncoderASet;
  _LeftEncoderBPrev = _LeftEncoderBSet;
}

int ParseEncoder() {
  if (_LeftEncoderAPrev && _LeftEncoderBPrev) {
    if (!_LeftEncoderASet && _LeftEncoderBSet) return 1;
    if (_LeftEncoderASet && !_LeftEncoderBSet) return -1;
  } else if (!_LeftEncoderAPrev && _LeftEncoderBPrev) {
    if (!_LeftEncoderASet && !_LeftEncoderBSet) return 1;
    if (_LeftEncoderASet && _LeftEncoderBSet) return -1;
  } else if (!_LeftEncoderAPrev && !_LeftEncoderBPrev) {
    if (_LeftEncoderASet && !_LeftEncoderBSet) return 1;
    if (!_LeftEncoderASet && _LeftEncoderBSet) return -1;
  } else if (_LeftEncoderAPrev && !_LeftEncoderBPrev) {
    if (_LeftEncoderASet && _LeftEncoderBSet) return 1;
    if (!_LeftEncoderASet && !_LeftEncoderBSet) return -1;
  }
}
// Interrupt service routines for the left motor's quadrature encoder
void HandleLeftMotorInterruptA() {
  _LeftEncoderBSet = digitalReadFast(c_LeftEncoderPinB);
  _LeftEncoderASet = digitalReadFast(c_LeftEncoderPinA);

  _LeftEncoderTicks += ParseEncoder();

  _LeftEncoderAPrev = _LeftEncoderASet;
  _LeftEncoderBPrev = _LeftEncoderBSet;
}
