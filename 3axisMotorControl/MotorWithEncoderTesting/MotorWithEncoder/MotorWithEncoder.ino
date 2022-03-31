
//turns motor and prints position of the turret
#include <Servo.h>
#include <Wire.h>
#include <I2CEncoder.h>

Servo turretServo; // create servo object to control the Turret servo
Servo reachServo; // create servo object to control the Turret servo
//
// Create an I2CEncoder for use
I2CEncoder encoder1;
I2CEncoder encoder2;
void setup() {
  Wire.begin(); // join i2c bus (address optional for master)
  Serial.begin(9600);
  
  // Initialize the encoder for a 269 motor
  encoder1.init(MOTOR_393_TORQUE_ROTATIONS, MOTOR_393_TIME_DELTA);
  encoder2.init(MOTOR_393_TORQUE_ROTATIONS, MOTOR_393_TIME_DELTA);


  turretServo.attach(9);                      // Attach left signal to pin 13
  reachServo.attach(10);
 
//  turretServo.writeMicroseconds(1600);         // 1.7 ms -> counterclockwise
}

void loop() {
Serial.print(encoder1.getSpeed()); // Get how fast it's rotating
Serial.print(", ");
Serial.print(encoder1.getPosition()); // Get it's current position
Serial.print("     ");
Serial.print(encoder2.getSpeed()); // Get how fast it's rotating
Serial.print(", ");
Serial.println(encoder2.getPosition()); // Get it's current position
}

 
