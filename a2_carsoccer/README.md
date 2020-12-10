# Assignment 2: Car Soccer

## Suihan Dong

## Draw the Boundary
Devide the boundary into four different parts, and create a vector list for each part. Choose the LINE_STRIP for the LINETYPE for the boundarys, so we can let the quickShapes_.DrawLines() function connect all the points in the vector list by itself.

## Draw the Goal
Create two for loops for drawing the goals, one for the column and one for the row. Using LINE for the LINETYPE for the goals, so we can let the quickShapes_.DrawLines() function only conncect the paor of the points that I give each time.

## Key Press
The ball's initial velocity is 0, when the space key is pressed, the ball's velocity is assigned to a Vector3(0, 0, 15). Thus the ball will move towards the player everytime when the space key is pressed.

## Update the Car
### car.h
1. add the data for the velocity of the car (v_car_), and initialized it with Vector3(0, 0, 0)
2. add the data for the thrust of the car (thrust_)
3. add the data for the speed of the car (speed_), initialized it with float value 0.0
4. add the function get_thrust() to access the thrust_ value
5. add the function speed() to access the speed_ value
6. add the function set_speed(float s) to update the speed_ when it changed
### car_soccer.c
In the UpdateSimulation(double timeStep) function
1. call the function Vector2 dir = joystick_direction() get the direction of the car by setting the turnRate to dir[0], which stores the value when the left or right key is pressed
2. get the all required values of the car
3. calculate the new values of the car
4. set the boundary, so the car cannot go beyond
5. update all values of the car

## Update the Ball
### ball.h
1. add the data for the velocity of the ball (velocity_), initial value is (0, 0, 0)
2. add the functions to get and set the ball's velocity

### car_soccer.c
In the UpdateSimulation(double timeStep) function
After the space key is presswd, the ball will move towards to the car, if the they are not contact, the ball continues moving. Once they are contact, there is no change with the car, but the ball's velocity will decrease.
    1. we check the distance between the ball's position and the car's position. If it is small or equal to the sum of the two radious, we update the ball's position to make the two values become equal by ball_pos = ball_pos + balltocar.ToUnit() * (ball_r + car_r - real_d).
    2. then we make the velocity of ball become the 80% of the original one
    3. we calculate the relaity velocity between the velocity of the ball and the car
    4. set the ball's velocity to the sum of the car's velocity and the relaity velocity
If the ball reaches the boundary of the field, then we set the inverse the direction of the ball's velocity to let it bounce and set the length of the velocity to the 0.8 of the original. Finally, if the ball hit the goal on either side, we reset the ball and the car.






