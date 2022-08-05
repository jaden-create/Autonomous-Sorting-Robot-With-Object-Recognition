#include <Servo.h>
#include <Wire.h>
#include <I2CEncoder.h>
#include "Arduino.h"

#include <digitalWriteFast.h>  // library for high performance reads and writes by jrraines



////pixy variables///////
#include <Pixy2.h>

int column1 = 29;
int column2 = 61;
int column3 = 105;
int column4 = 160;
int column5 = 210;
int column6 = 260;

int rowDivide = 100; // where the row divides


int threshold = 30; // how much different from test points a ball can be, default 30//was 50


int reader = 0;
int p = 1;
// This is the main Pixy object
Pixy2 pixy;
int row1type[6] = {0, 0, 0, 0, 0, 0};
int row2type[6] = {0, 0, 0, 0, 0, 0};
String lcdRow1;
String lcdRow2;
/////////////////////////




const int startButtonPin = 49;//53;
//int startButtonState;

const int buttonPin = 7;     // the number of the pushbutton limit pin for reach armint buttonState;         // variable for reading the pushbutton status
int buttonState;         // variable for reading the pushbutton status

int mySensVals[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}; //replace with pixy2 readings


const int lowerButtonPin = 2;     // the number of the pushbutton limit pin for reach armint buttonState;         // variable for reading the pushbutton status
int lowerButtonState;         // variable for reading the pushbutton status


const int limitPin = 6;     // the number of the pushbutton limit pin for reach armint buttonState;         // variable for reading the pushbutton status
int limitState;         // variable for reading the pushbutton status

const int openPin = 8;     // the number of the pushbutton limit pin for reach armint buttonState;         // variable for reading the pushbutton status
int openState;         // variable for reading the pushbutton status


const int relayPin = 3;



float reachPos;//position of reach arm
float liftPos;
float openPos;

Servo turretServo; // create servo object to control the Turret servo
Servo reachServo; // create servo object to control the Turret servo
Servo liftServo; // create servo object to control the Turret servo
Servo grabServo; // create servo object to control the Turret servo
Servo openServo; // opening arm that descends

//


I2CEncoder encoderReach;//reach
//I2CEncoder encoderLift;//lift axis
I2CEncoder encoderOpen;//open servo arm


String readString;
String inputHold; // used when we need to store the readString and trim it, in order to use it in comparisons
int potValue;
void setup() {
  Wire.begin(); // join i2c bus (address optional for master) + this is needed for code to work

  // put your setup code here, to run once:
  Serial.begin(9600);

  pixy.init();/////////////////////////////////////////////////

  encoderReach.init(MOTOR_393_TORQUE_ROTATIONS, MOTOR_393_TIME_DELTA);
  //encoderLift.init(MOTOR_393_TORQUE_ROTATIONS, MOTOR_393_TIME_DELTA);
  encoderOpen.init(MOTOR_393_TORQUE_ROTATIONS, MOTOR_393_TIME_DELTA);

  turretServo.attach(9);
  reachServo.attach(10);
  liftServo.attach(11);
  grabServo.attach(5);
  openServo.attach(4);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(lowerButtonPin, INPUT_PULLUP);
  pinMode(startButtonPin, INPUT_PULLUP);

  pinMode(limitPin, INPUT_PULLUP);
  pinMode(openPin, INPUT_PULLUP);

  pinMode(relayPin, OUTPUT);

  // pixy.init(); // go to 561 5.81 for reading //////////////////////////////************************** + do by shape not just color reading
}
void loop() {
  int startButtonState = digitalRead(startButtonPin); // make this void readCarton();

  if (startButtonState == LOW) {
    liftReturnToBase();
    reachReturnToBase();
    openCarton();
    readCarton();
    delay(500);

    //////////// put pixy code into master array///////////// temp comment
    for (int i = 11; i >= 0; i--) {
      Serial.println(mySensVals[i]);

    }
    for (int i = 11; i >= 6; i--) {
      mySensVals[i] = row2type[i - 6];
    }
    for (int i = 5; i >= 0; i--) {
      mySensVals[i] = row1type[i];
    }
    //    for (int i = 11; i >= 0; i--) {
    //      Serial.print(i + 1);
    //      Serial.print("   ");
    //      Serial.println(mySensVals[i]);
    //      pickUp(i + 1, mySensVals[i]);
    //    }



    for (int i = 0; i <= 11; i++) {
      Serial.print(i + 1);
      Serial.print("   ");
      Serial.println(mySensVals[i]);
      pickUp(i + 1, mySensVals[i]);
    }

    ////////////////////////////////////////////////////


    Serial.print("\n");




    //temp commen
    //    for (int i = 11; i >= 0; i--) {
    //      Serial.print(i + 1);
    //      Serial.print("   ");
    //      Serial.println(mySensVals[i]);
    //      pickUp(i + 1, mySensVals[i]);
    //    }



    delay(500);

  }

  takeInput();

}



void readCarton() {
  float readNum = 5.86;
  turretGoTo(565);
  reachGoTo(readNum);
  delay(500);
  reachGoToExact(readNum);

  delay(6000);
  pixyTakeShot();
}



void takeInput() {
  while (Serial.available()) {
    char c = Serial.read();  //gets one byte from serial buffer
    readString += c; //makes the string readString
    delay(2);  //slow looping to allow buffer to fill with next character
    //  Serial.print(" fdofds");
  }

  if (readString.length() > 0) {
    Serial.println(readString);

    inputHold = readString;
    inputHold.trim();
    if (inputHold.equalsIgnoreCase("stats")) {
      potValue = analogRead(A0);//variable for position of turret
      Serial.print(potValue);
      Serial.print("     ");
      Serial.print(encoderReach.getPosition()); // Get it's current position
      //Serial.print("     ");
      //Serial.print(encoderLift.getPosition()); // Get it's current position
      Serial.print("     ");
      Serial.print(encoderOpen.getPosition()); // Get it's current position
    }
    else if (inputHold.equalsIgnoreCase("reset")) {
      liftReturnToBase();
      reachReturnToBase();

    }
    else if (inputHold.equalsIgnoreCase("base")) {
      //openCarton();
      openReturnToBase();
    }
    else if (inputHold.equalsIgnoreCase("open")) {
        grabServo.writeMicroseconds(1600);         // 1.7 ms -> counterclockwise // was 1400 but motor was not strong enough
        delay(9000);
        grabServo.writeMicroseconds(1500);         // 1.7 ms -> counterclockwise // was 1400 but motor was not strong enough


      //openCarton();
      //openReturnToBase();
    }
    else if (inputHold.equalsIgnoreCase("read")) {
      readCarton();
    }
    else if (inputHold.equalsIgnoreCase("shot")) {
      pixyTakeShot();
    }
    else if (inputHold.equalsIgnoreCase("test")) {//just combining arrays into one aray
      for (int i = 11; i >= 0; i--) {
        Serial.println(mySensVals[i]);

      }
      for (int i = 11; i >= 6; i--) {
        mySensVals[i] = row2type[i - 6];
      }
      for (int i = 5; i >= 0; i--) {
        mySensVals[i] = row1type[i];
      }
      for (int i = 11; i >= 0; i--) {
        Serial.print(i + 1);
        Serial.print("   ");
        Serial.println(mySensVals[i]);
        //pickUp(i + 1, mySensVals[i]);
      }
    }
    else if (inputHold.equalsIgnoreCase("seq")) {
      for (int i = 0; i <= 11; i++) {
        Serial.print(i + 1);
        Serial.print("   ");
        Serial.println(mySensVals[i]);
        pickUp(i + 1, 0);//mySensVals[i]);
      }
    }
    else {

      int cartonNum = inputHold.toInt();
      //dropOff(cartonNum);
      pickUp(cartonNum, 1);
      //liftGoTo(cartonNum);
      //liftPickUp();

      //openCarton();

    }
    readString = ""; //empty for next input

  }
}

void openCarton() {
  liftReturnToBase();
  reachReturnToBase();
  turretGoTo(485);
  reachGoTo(1.82);//2.4);//was 2
  //put arm in position

  grabServo.writeMicroseconds(1600);         // 1.7 ms -> counterclockwise // was 1400 but motor was not strong enough
  delay(9000);
  grabServo.writeMicroseconds(1500);         // 1.7 ms -> counterclockwise // was 1400 but motor was not strong enough

  openReturnToBase();
  //openGoTo(-0.6);
  delay(500);
  openGoTo(-3);
  delay(500);//remove eventually
  reachGoTo(5);
  openGoTo(-5);//maybe not necessary
  reachGoTo(7);
  openGoTo(-4);//maybe not necessary
  reachGoTo(8.2);
  delay(300);
  openGoTo(-10);
  reachReturnToBase();

  //  Serial.print("grabbing");
  //  delay(7000);
  //////grabServo.writeMicroseconds(1500);         // 1.7 ms -> counterclockwise // was 1400 but motor was not strong enough
  //  grabServo.writeMicroseconds(1400);         // 1.7 ms -> counterclockwise // was 1400 but motor was not strong enough
  //  delay(7000);
  //  grabServo.writeMicroseconds(1500);         // 1.7 ms -> counterclockwise // was 1400 but motor was not strong enough
  //  Serial.print("done");



}






void pickUp(int location, int type) { // 2,3 = tailings, 1 = yellow, 0 =white
  //****************************************************************************** add conditions to skip tailing
  if (type != 2 && type != 3) {//added the type 3 condition
    liftReturnToBase();
    reachReturnToBase();
    String CartonPos = CartonPositions(location);
    int spaceIndex = CartonPos.indexOf(" ");
    int turretNum = CartonPos.substring(0, spaceIndex).toInt(); //first number of string
    float reachNum = CartonPos.substring(spaceIndex + 1, -1).toFloat(); //second number of string
    Serial.print(turretNum);
    Serial.print("  ");
    Serial.println(reachNum);
    turretGoTo(turretNum);
    reachGoTo(reachNum);
    //liftGoTo(-17);//////////////////////////////////////////////////////////////////////////////////////////////////
    liftPickUp();
    vaccumOn();
    delay(2000);//replace with drop off
    liftReturnToBase();
    //potentially add turretGoTo() a position length that is good for dropping off


    //turretGoTo(260);
    dropOff(type);


    // if type is gold... drop off ggold
    // if silver ...
    vaccumOff();
    // return to Base on lift and then after on reach
    delay(4000);

    
    if (type == 1) { //gold
    liftDown();

    delay(2500);
    reachBackward();

    delay(1000);
    liftReturnToBase();
    reachReturnToBase();

  }
    

  }
}

void dropOff(int type) {
  reachGoTo(9);
  int newPos = 500; // may have to change
  if (type == 1) { //gold
    newPos = 250;
    Serial.print("gold");

  }

  if (type == 0) {
    newPos = 165;
    Serial.print("silver");

  }
  Serial.print("dropping off at");
  Serial.println(newPos);


  potValue = analogRead(A0);

  Serial.println(potValue);
  if (newPos > potValue) { //if new position is to right of current position
    newPos -= 30;//was 30, but need to be closer when slow goes
    while (newPos > potValue) {
      turretCounterClockwise();//turn clockwise towards new position
      potValue = analogRead(A0);//variable for position of turret
      Serial.println(potValue);
      //      printStats();//print out current speed and position
    }

  }
  else if (newPos < potValue) { //if new position is to left of current position
    newPos += 30;
    while (newPos < potValue) {
      turretClockwise();// turn in counterClockwise position towards new position
      potValue = analogRead(A0);//variable for position of turret
      Serial.println(potValue);
      //      printStats();// print out current speed and position
    }
  }
  turretServo.writeMicroseconds(1500);//stop
  Serial.println("doneDropOff");
}






void vaccumOn() {
  digitalWrite(relayPin, HIGH);
}
void vaccumOff() {
  digitalWrite(relayPin, LOW);
}


void reachReturnToBase() { //will make it go to base position based off of sensors
  Serial.println("reach returning to base");

  buttonState = digitalRead(buttonPin);
  while (buttonState == HIGH) {
    reachBackward();
    buttonState = digitalRead(buttonPin);
  }
  reachServo.writeMicroseconds(1500);//stop
  encoderReach.zero();
  Serial.println("reach returned");
}



void reachGoTo(float newPos) {//full extension is approx 2.5 ticks, will probably have to reduce gear ratio
  Serial.println("going");
  float reachPos = encoderReach.getPosition();//variable for position of reach arm
  if (newPos > reachPos) { //if new position is ahead of current position
    while (newPos > reachPos) {
      reachPos = encoderReach.getPosition();//variable for position of reach arm
      reachForward();//move forward towards new position
      Serial.println(reachPos);
    }
  }
  else if (newPos < reachPos) { //if new position is to left of current position
    while (newPos < reachPos) {
      reachPos = encoderReach.getPosition();//variable for position of reach arm
      reachBackward();// move in backward towards new position
      Serial.println(reachPos);
    }
  }
  reachServo.writeMicroseconds(1500);//stop
}



void reachGoToExact(float newPos) {
  reachPos = encoderReach.getPosition();
  while (int(reachPos * 10) != int(newPos * 10) ) {//was *100 but too accurate to find
    reachPos = encoderReach.getPosition();//variable for position of reach arm



    if ((reachPos) < (newPos)) { //if it is greter than 10 away form target
      reachForward();
      delay(10);
      reachServo.writeMicroseconds(1500);//stop
      delay(10);
      //Serial.println(potValue);
    }

    else if ((reachPos) > (newPos)) { // if it is more than ten below the target
      reachBackward();
      delay(10);
      reachServo.writeMicroseconds(1500);//stop
      delay(10);
      //Serial.println(potValue);
    }


    turretServo.writeMicroseconds(1500);//stop
    delay(250);//potentially change
    reachPos = encoderReach.getPosition();
    Serial.println(int(reachPos * 10));

    Serial.print(int(newPos * 10));
    Serial.print(" ");

  }
  Serial.println("arrived");
}




void reachForward() { //move motor counterClockwise
  reachServo.writeMicroseconds(1700);  //was 1650       // 1.7 ms -> forward
}

void reachBackward() { //move motor counterClockwise
  reachServo.writeMicroseconds(1200);    //was 1300 but troubles moving it sometimes     // 1.7 ms -> backward
}






void turretGoTo(int goToPos) {//full 360 degrees is 11ticks
  potValue = analogRead(A0);
  int newPos = goToPos;

  Serial.println(potValue);
  if (newPos > potValue) { //if new position is to right of current position
    newPos -= 30;//was 30, but need to be closer when slow goes
    while (newPos > potValue) {
      turretCounterClockwise();//turn clockwise towards new position
      potValue = analogRead(A0);//variable for position of turret
      Serial.println(potValue);
      //      printStats();//print out current speed and position
    }

  }
  else if (newPos < potValue) { //if new position is to left of current position
    newPos += 30;
    while (newPos < potValue) {
      turretClockwise();// turn in counterClockwise position towards new position
      potValue = analogRead(A0);//variable for position of turret
      Serial.println(potValue);
      //      printStats();// print out current speed and position
    }
  }
  Serial.println("done initial turn");
  turretServo.writeMicroseconds(1500);//stop
  delay(500);
  goToDesiredPosNew(goToPos);
}







void goToDesiredPosNew(int newPos) {
  potValue = analogRead(A0);
  while (potValue != newPos) {
    potValue = analogRead(A0);



    if ((potValue) < (newPos)) { //if it is greter than 10 away form target
      turretCounterClockwise();
      delay(10);
      turretServo.writeMicroseconds(1500);//stop
      delay(10);
      //Serial.println(potValue);
    }

    else if ((potValue) > (newPos)) { // if it is more than ten below the target
      turretClockwise();
      delay(10);
      turretServo.writeMicroseconds(1500);//stop
      delay(10);
      //Serial.println(potValue);
    }



    turretServo.writeMicroseconds(1500);//stop
    delay(250);//potentially change
    potValue = analogRead(A0);
    Serial.println(potValue);


  }
  //////////added code to add precision//////////
  delay(2000);
  Serial.println("Checking if need to go again");
  potValue = analogRead(A0);
  if (potValue != newPos) {
    Serial.println("Going again");
    goToDesiredPosNew(newPos);
  }
  //////////////////////////////////////

  Serial.println("at location");
}




void liftReturnToBase() { //will make it go to base position based off of sensors
  Serial.println("lift returning to base");
  limitState = digitalRead(limitPin);
  while (limitState == HIGH) {
    liftUp();
    limitState = digitalRead(limitPin);
  }
  liftServo.writeMicroseconds(1500);//stop
  //encoderLift.zero();
  Serial.println("returned");
}
void liftPickUp() { //will make it go to base position based off of sensors
  Serial.println("lift returning to pickUp");
  lowerButtonState = digitalRead(lowerButtonPin);
  while (lowerButtonState == HIGH) {
    liftDown();
    lowerButtonState = digitalRead(lowerButtonPin);
  }
  liftServo.writeMicroseconds(1500);//stop
  //encoderLift.zero();
  Serial.println("returned");
}

//
//void liftGoTo(int newPos) {//full extension is approx 2.5 ticks, will probably have to reduce gear ratio
//  Serial.println("going");
//  liftPos = encoderLift.getPosition();//variable for position of reach arm
//  if (newPos > liftPos) { //if new position is ahead of current position
//    while (newPos > liftPos) {
//      liftPos = encoderLift.getPosition();//variable for position of reach arm
//      liftUp();// move in backward towards new position
//
//      Serial.println(liftPos);
//    }
//  }
//  else if (newPos < liftPos) { //if new position is to left of current position
//    while (newPos < liftPos) {
//      liftPos = encoderLift.getPosition();//variable for position of reach arm
//      liftDown();//move forward towards new position
//
//      Serial.println(liftPos);
//    }
//  }
//  liftServo.writeMicroseconds(1500);//stop
//
//}

void openReturnToBase() { //will make it go to base position based off of sensors
  Serial.println("opener returning to base");
  openState = digitalRead(openPin);
  while (openState == HIGH) {
    openDown();
    openState = digitalRead(openPin);
  }
  openServo.writeMicroseconds(1500);//stop
  encoderOpen.zero();
  Serial.println("returned");
}


void openGoTo(float newPos) {//full extension is approx 2.5 ticks, will probably have to reduce gear ratio
  Serial.println("going");
  openPos = encoderOpen.getPosition();//variable for position of reach arm
  if (newPos > openPos) { //if new position is ahead of current position
    while (newPos > openPos) {
      openPos = encoderOpen.getPosition();//variable for position of reach arm
      openDown();//move forward towards new position
      Serial.println(openPos);
    }
  }
  else if (newPos < openPos) { //if new position is to left of current position
    while (newPos < openPos) {
      openPos = encoderOpen.getPosition();//variable for position of reach arm
      openUp();// move in backward towards new position
      Serial.println(openPos);
    }
  }
  openServo.writeMicroseconds(1500);//stop

}

void liftDown() { //move motor counterClockwise
  liftServo.writeMicroseconds(1650);         // 1.7 ms -> counterclockwise // was 1400 but motor was not strong enough
}

void liftUp() { //move motor counterClockwise
  liftServo.writeMicroseconds(1000);         // 1.3 ms -> clockwise// was 1400 but motor was not strong enough
}
void openDown() { //move motor counterClockwise
  openServo.writeMicroseconds(1600);         // 1.7 ms -> counterclockwise // was 1400 but motor was not strong enough
}

void openUp() { //move motor counterClockwise
  openServo.writeMicroseconds(1350);         // 1.3 ms -> clockwise// was 1400 but motor was not strong enough
}



int slowVar = 100;//was 120

void turretCounterClockwise() { //move motor counterClockwise
  turretServo.writeMicroseconds(1650);         // 1.7 ms -> counterclockwise // was 1400 but motor was not strong enough
}

void turretClockwise() { //move motor counterClockwise
  turretServo.writeMicroseconds(1350);         // 1.3 ms -> clockwise// was 1400 but motor was not strong enough
}

void turretCounterClockwisePower() { //move motor counterClockwise
  turretServo.writeMicroseconds(1700);        // 1.7 ms -> counterclockwise // was 1400 but motor was not strong enough
}

void turretClockwisePower() { //move motor counterClockwise
  turretServo.writeMicroseconds(1300);       // 1.3 ms -> clockwise// was 1400 but motor was not strong enough
}

//123456
//789101112

String CartonPositions(int num) { //return string of two positions
  if (num == 1) {
    return "562 9.2";
  }
  else if (num == 2) {//
    return "534 8.83";
  }
  else if (num == 3) {//meh
    return "506 8.70";
  }
  else if (num == 4) {//skip
    return "474 8.70";
  }
  else if (num == 5) {//meh
    return "450 8.75";
  }
  else if (num == 6) {//skip
    return "420 9.3";
  }
  else if (num == 7) {//good
    return "572 6.73";
  }
  else if (num == 8) {//skip
    return "547 6.01";
  }
  else if (num == 9) {//good
    return "508 5.82";
  }
  else if (num == 10) {//good
    return "475 5.82";
  }
  else if (num == 11) {//good
    return "441 6.06";
  }
  else if (num == 12) {//good
    return "413 6.63";
  }
}



///////////////////////////////////////////////////// pixy codes




void pixyTakeShot() {
  Serial.println("taking Shot");
  int i;
  for (i = 0; i < 6; i++) {
    row1type[i] = 0;
    row2type[i] = 0;
  }
  Serial.println("getting blocks");
  pixy.ccc.getBlocks();
  Serial.println(" blocks gotten");

  if (pixy.ccc.numBlocks)
  {
    Serial.println("if statement passed");

    for (i = 0; i < pixy.ccc.numBlocks; i++)
    {
      point_assign(pixy.ccc.blocks[i].m_x, pixy.ccc.blocks[i].m_y, pixy.ccc.blocks[i].m_signature);
    }

  }
  printRows();
  reader = Serial.read();
  reader = Serial.read();

}



void point_assign(int x, int y, int type) {
  if (y > rowDivide) { //if bottom row
    if ((x > (column1 - threshold)) && (x < (column1 + threshold))) { // if r2c1
      row2type[0] = type;
    }
    else if ((x > (column2 - threshold)) && (x < (column2 + threshold))) { // if r2c2
      row2type[1] = type;
    }
    else if ((x > (column3 - threshold)) && (x < (column3 + threshold))) { // if r2c3
      row2type[2] = type;
    }
    else if ((x > (column4 - threshold)) && (x < (column4 + threshold))) { // if r2c4
      row2type[3] = type;
    }
    else if ((x > (column5 - threshold)) && (x < (column5 + threshold))) { // if r2c5
      row2type[4] = type;
    }
    else if ((x > (column6 - threshold)) && (x < (column6 + threshold))) { // if r2c6
      row2type[5] = type;
    }
  }
  else if (y < rowDivide) { //if bottom row
    if ((x > (column1 - threshold)) && (x < (column1 + threshold))) { // if r1c1
      row1type[0] = type;
    }
    else if ((x > (column2 - threshold)) && (x < (column2 + threshold))) { // if r1c2
      row1type[1] = type;
    }
    else if ((x > (column3 - threshold)) && (x < (column3 + threshold))) { // if r1c3
      row1type[2] = type;
    }
    else if ((x > (column4 - threshold)) && (x < (column4 + threshold))) { // if r1c4
      row1type[3] = type;
    }
    else if ((x > (column5 - threshold)) && (x < (column5 + threshold))) { // if r1c5
      row1type[4] = type;
    }
    else if ((x > (column6 - threshold)) && (x < (column6 + threshold))) { // if r1c6
      row1type[5] = type;
    }
  }




}



// 1 = Y
// 2 //3 = T




String typetrans(int type) {
  if (type == 1) {
    return ("Y");
  }
  else if (type == 2 || type == 3) {
    return ("T");
  }
  else {
    return ("W");
  }
}

void printRows() {
  lcdRow1 = "";
  lcdRow2 = "";
  Serial.print("\n");
  Serial.print("Row 1 corresponding types");
  for (int i = 0; i < 6; i++) {
    Serial.print(" ");
    Serial.print(typetrans(row1type[i]));
    lcdRow1 += " ";
    lcdRow1 += typetrans(row1type[i]);
  }
  Serial.print("\n");
  Serial.print("Row 2 corresponding types");
  for (int i = 0; i < 6; i++) {
    Serial.print(" ");
    Serial.print(typetrans(row2type[i]));
    lcdRow2 += " ";
    lcdRow2 += typetrans(row2type[i]);
  }

  //  lcd.clear();
  //  lcd.setCursor(0, 0);
  //Serial.print(lcdRow1);

  //lcd.setCursor(0, 1);
  //Serial.print(lcdRow2);
}
