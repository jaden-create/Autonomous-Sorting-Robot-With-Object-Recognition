/*
 Robotics with the BOE Shield – ServosOppositeDirections
 Generate a servo full speed counterclockwise signal with pin 13 and
 full speed clockwise signal with pin 12.
 */
 
#include <Servo.h>                           // Include servo library
 
Servo servoLeft;                             // Declare left servo signal

 
void setup()                                 // Built in initialization block
{
  servoLeft.attach(10);                      // Attach left signal to pin 13
 

}  
 
void loop()                                  // Main loop auto-repeats
{                                            // Empty.
   servoLeft.writeMicroseconds(1700);  // 1.7 ms -> counterclockwise

   delay(30);
  servoLeft.writeMicroseconds(1300);        // 1.3 ms -> clockwise
  delay(30);
}
