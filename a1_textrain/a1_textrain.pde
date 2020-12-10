/**
    CSci-4611 Assignment #1 Text Rain
**/


import processing.video.*;
import java.util.*; 


// Global variables for handling video data and the input selection screen
String[] cameras;
Capture cam;
Movie mov;
PImage inputImage;
boolean inputMethodSelected = false;
int threshold = 128;
String[] arr = {"L", "i", "f", "e", "i", "s", "b", "u", "t", "a", "n", "e", "m", "p", "t", "y", "d", "r", "e", "a", "m"};
//Collections.shuffle(arr);
int pos_arr[] = new int[64];
int charIndex = 0;
int xpos = 0;
int ypos = 100;
int n = 0;
int index1D = 0;
ArrayList<String> alst = new ArrayList();

//int i = 0;


void setup() {
  size(1280, 720);  
  inputImage = createImage(width, height, RGB);
  
  // HACK: To force offline mode because the camera isn"t working on laptop
  println("Offline mode selected.");
  mov = new Movie(this, "TextRainInput.mov");
  mov.loop();
  inputMethodSelected = true;
}


// Returns: thresholded color (black or white)
color thresholdPixel(color inputPixel) {
  color c = color(0);
  if (green(inputPixel) > 128) {
    c = color(255);
  } 
  return c;
}


void draw() {
  // When the program first starts, draw a menu of different options for which camera to use for input
  // The input method is selected by pressing a key 0-9 on the keyboard
  if (!inputMethodSelected) {
    cameras = Capture.list();
    int y=40;
    text("O: Offline mode, test with TextRainInput.mov movie file instead of live camera feed.", 20, y);
    y += 40; 
    for (int i = 0; i < min(9,cameras.length); i++) {
      text(i+1 + ": " + cameras[i], 20, y);
      y += 40;
    }
    return;
  }


  // This part of the draw loop gets called after the input selection screen, during normal execution of the program.

  
  // STEP 1.  Load an image, either from a movie file or from a live camera feed. Store the result in the inputImage variable
  
  if ((cam != null) && (cam.available())) {
    cam.read();
    inputImage.copy(cam, 0,0,cam.width,cam.height, 0,0,inputImage.width,inputImage.height);
  }
  else if ((mov != null) && (mov.available())) {
    mov.read();
    inputImage.copy(mov, 0,0,mov.width,mov.height, 0,0,inputImage.width,inputImage.height);
  }


  // Fill in your code to implement the rest of TextRain here..

  // Tip: This code draws the current input image to the screen
  set(0, 0, inputImage);
  for(int p = 0; p < arr.length; p++) {
    alst.add(arr[p]);
  }
  Collections.shuffle(alst);
  
  int i = 0;
  for(int idx = 0; idx < inputImage.width/20; idx++) {
    pos_arr[idx] = i;
    i = i + 20;
  }
  textSize(24);
  fill(0,102,153,204);
  
  inputImage.loadPixels();
  int k = 0;
  int h = 0;
  int ran = 0;
  while(h < 64 && ypos < inputImage.height){  
     text(arr[k], pos_arr[h], ypos);
     //ran = int(random(arr.length));  
     int cur = pos_arr[h] + inputImage.width * ypos;
     //println(pos_arr[h], ypos,cur);
     
     if (thresholdPixel(inputImage.pixels[cur]) == color(0)){
       ypos--;
     } else {
       ypos++;
     }
     h++;
  }
  
  if (ypos == inputImage.height) {
    ypos = 0;
  }
  inputImage.updatePixels();
}



void keyPressed() {
  
  if (!inputMethodSelected) {
    // If we haven"t yet selected the input method, then check for 0 to 9 keypresses to select from the input menu
    if ((key >= '0') && (key <= '9')) { 
      int input = key - '0';
      if (input == 0) {
        println("Offline mode selected.");
        mov = new Movie(this, "TextRainInput.mov");
        mov.loop();
        inputMethodSelected = true;
      }
      else if ((input >= 1) && (input <= 9)) {
        println("Camera " + input + " selected.");           
        // The camera can be initialized directly using an element from the array returned by list():
        cam = new Capture(this, cameras[input-1]);
        cam.start();
        inputMethodSelected = true;
      }
    }
    return;
  }


  // This part of the keyPressed routine gets called after the input selection screen during normal execution of the program
  // Fill in your code to handle keypresses here..
  
  if (key == CODED) {
    if (keyCode == UP) {
      // up arrow key pressed
      threshold += 3;
    }
    else if (keyCode == DOWN) {
      // down arrow key pressed
      threshold -= 3;
    }
  }
  else if (key == ' ') {
    // space bar pressed
    inputImage.loadPixels();
    for(int i = 0; i< inputImage.width; i++) {
      for(int j = 0; j < inputImage.height; j++) {
        index1D = i + (inputImage.width * j);
        //println("index1D" + index1D);
        color c = thresholdPixel(inputImage.pixels[index1D]);
        
        inputImage.pixels[index1D] = c;  
      }
    }
    inputImage.updatePixels();
  } 
  
}
