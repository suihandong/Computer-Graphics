# CSci-4611 Assignment 3:  Earthquake

## Suihan Dong

## Earth Class
Instead of using 10 slices and 10 stacks, I used 30 slices and 30 stacks. 
Calculate the Point for the sphere use the function LatLongToSphere(), and mapped all points to the correspond texture.
I add a function Globe(float alpha), this function is used to do the transform between the sphere and the plane when the globe botton is pressed. In this function, we called the Lerp() function to make the changes between the plane and the sphere smooth. When call the Lerp(), the start point is the point on the plane, the end point is the point on the sphere. We pass in the alpha value wich is associated with the dt in the QuakeApp::UpdateSimulation(double dt). The alpha vaule will be updated each frame.

## Quake_app Class
1. Add a function Color(double mag), this function deciede the color of the earthquake that will draw on the screen. This function takes in the magnitude of the earthquake as the parameter. It will return the color that is correspond with the earthquake's magnitude value. The magnitude is divided into 10 levels, the level is increased with the magnitude value. And the color is from yellow to red with the level from lower to higher. For example, if the level is 1, the color is yellow, if the level is 10, the color is red.
2. Add a function Radious(double mag),this function deciede the size of the earthquake that will draw on the screen. This function takes in the magnitude of the earthquake as the parameter. It will return the size that is correspond with the earthquake's magnitude value. The magnitude is divided into 10 levels, the level is increased with the magnitude value. And the size is from 0.012 to 0.0345 with the level from lower to higher. For example, if the level is 1, the size is 0.012, if the level is 10, the size is 0.0345.
3. In the UpdateSimulation(double dt), we use a for loop to find all the earthquakes that have happened within the past one year of the current visualization time. We calculate the position of the earthquakes for both on the plane and the sphere, get the color and size of the earthquake. We add the color to the color_list, add size to the radious_list, add sphere positon to the sphere_position_list, and the plane position to the plane_position_list. If the earth is in globe mode, I add 0.05 to the alpha each frame until it reaches to 1. If not, I decrease 0.05 to the alpha each frame until it reaches to 0. Also, If globe_mode, set a rotation rate for the earth. If not, the rotation rate is zero.
4. Draw the earthquakes. Since I have already set the list of colors, sizes and the positions of the earthquakes. I loop throught the list and draw all the earthquakes that should show up. Clear the list at the end of the frame. 
