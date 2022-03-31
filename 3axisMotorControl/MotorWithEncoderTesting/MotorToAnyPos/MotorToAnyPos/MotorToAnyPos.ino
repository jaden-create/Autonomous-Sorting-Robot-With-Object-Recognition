
//turns turret motor to assigned position


String readString;


#include <Servo.h>
#include <Wire.h>
#include <I2CEncoder.h>
float turretPos;
int reader = 0;
Servo turretServo; // create servo object to control the Turret servo
//
// Create an I2CEncoder for use
I2CEncoder encoder;

void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);

  // Initialize the encoder for a 269 motor
  encoder.init(MOTOR_393_TORQUE_ROTATIONS, MOTOR_393_TIME_DELTA);


  turretServo.attach(9);                      // Attach left signal to pin 13

}

void loop() {

  while (Serial.available()) {
    char c = Serial.read();  //gets one byte from serial buffer
    readString += c; //makes the string readString
    delay(2);  //slow looping to allow buffer to fill with next character
  }

  if (readString.length() > 0) {
    Serial.println(readString);  //so you can see the captured string
    int n = readString.toInt();  //convert readString into a number
    turretGoTo(n);
//    //printStats();
    readString=""; 
  }
}








  void turretGoTo(int newPos) {
    turretPos = encoder.getPosition();//variable for position of turret
    if (newPos > turretPos) { //if new position is to right of current position
      while (newPos > turretPos) {
        turretPos = encoder.getPosition();//variable for position of turret
        Clockwise();//turn clockwise towards new position
        printStats();//print out current speed and position
      }
    }
    else if (newPos < turretPos) { //if new position is to left of current position
      while (newPos < turretPos) {
        turretPos = encoder.getPosition();//variable for position of turret
        counterClockwise();// turn in counterClockwise position towards new position
        printStats();// print out current speed and position
      }
    }
    turretServo.writeMicroseconds(1500);
    

  }

  void printStats() {
    Serial.print(encoder.getSpeed()); // Get how fast it's rotating
    Serial.print(", ");
    Serial.println(encoder.getPosition()); // Get it's current position
  }

  void counterClockwise() { //move motor counterClockwise
    turretServo.writeMicroseconds(1700);         // 1.7 ms -> counterclockwise
  }

  void Clockwise() { //move motor counterClockwise
    turretServo.writeMicroseconds(1300);         // 1.7 ms -> counterclockwise
  }
