# 3-Axis-Autonomous-Arm-motor-set-up-

Final Product: https://youtu.be/mjW2LClnK60

Made for the 2022 Skills Ontario Competition, in which my team came 6th. My team worked on the remote controled section of the competition while I solely did the autonomus portion of the competition.

More details:

Full length Video: https://youtu.be/QxDmXnuPQVc

This robot, which started in November and was finished by June, was for the 2022 Skills Ontario robotics competition (https://www.youtube.com/watch?v=4GhAACrLUkg). I was tasked solely with making the autonomous robot portion of the competition. It required I open the egg carton and put the yellow balls(gold) into the gold bin and the white balls(silver) into the silver bin to score points, all while avoiding the useless spools(tailings). We ended up coming 5th in Ontario for the combined score of both portions of the competition.

I decided to make a three axis arm that had three objectives:

1. Open the carton with sharpened screws and a claw.
2. Read the Carton using the Pixy 2 Camera(readings displayed in the video) in order to create an array that contains the co-ordinates of each object in the egg carton.
3. Move the balls into the proper places using the Roomba vacuum and the arm.

The main problem I came across was moving my arm to the exact precise location I required. This was a problem because I was working with primarily VEX components that could not offer such precision with motor encoders. To account for the encoder values drifting I installed limit switches that would reset the encoders when they reach a base position(inspired by 3D printers). And for the rotating base(which had slack against the motor gear, negating the use of an motor encoder and allowing for momentum to carry the arm out of position), I used a potentiometer with an algorithm that would get the turret close to the required location quickly and then take microseconds long jumps until it was directly on the target. If the base rotation was off by a small angle that inaccuracy would become much more drastic as the arm reached out.
