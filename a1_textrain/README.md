
## Algorithm for a1_textrain
Suihan Dong

#### Text_rain
+ choose the sentence I want to use for the project and made the sentence into an array list so I can choose the text to shows up on the screen randomly.
+ choose the font size for the text and then decided the distance in between each text to avoid the texe cover each other.
+ calculate how many letters could show up to avoid go beyond the limit.
+ set the initial row value of location of the text at row 0 so the text will falling out in to the screen after the program begin
+ when the text rain is falling, calculate the 1D index for each text and check the color of the pixel. If the color is black then the text go back for 1, otherwise the text contine falling. Check the color by compare the grayvalue with 128, if larger then set it to the white color(color (0)), else set it to black, color(0) .
+ if the text rain reached at the bottom of the frame, set it back to the start point.


#### Key Pressed
+ if the UP botton pressed, increase the difference
+ if the DOWN botton pressed, decrease the difference
+ when the SPACE botton pressed, if the greyvalue of the pixel is larger than 128, set it to white, else set it to black

