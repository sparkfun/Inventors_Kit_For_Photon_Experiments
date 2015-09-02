/*  SparkFun Inventor's Kit for Photon
        Experiment 4 - Part 1
        This sketch was written by SparkFun Electronics
      Ben Leduc-Mills
      August 31, 2015
      https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments

      This is an example sketch using buttons and a potentiometer to change the color and brightness of the Photon RedBoard onboard LED.

      Development environment specifics:
      Particle Build environment (https://www.particle.io/build)
      Particle Photon RedBoard
      Released under the MIT License(http://opensource.org/licenses/MIT)
 */
int redButton = D2; // declare variable for red button
int greenButton = D3; // declare variable for green button
int blueButton = D4; // declare variable for blue button

int potentiometer = A0; // declare variable for potentiometer
int colorMode; // declare variable to keep track of color
int potValue = 0; // // declare variable for the value of the potentiometer

void setup() {

  RGB.control(true); // command to control the RGB led on the Photon
  // buttons need an internal pullup resistor - see below for notes
  pinMode(redButton, INPUT_PULLUP);
  pinMode(greenButton, INPUT_PULLUP);
  pinMode(blueButton, INPUT_PULLUP);
  pinMode(potentiometer, INPUT); // potentiometers are an analog input
  colorMode = 0;

}

void loop() {

  // change colorMode variable depending on which button was pressed
  if (digitalRead(redButton) == LOW) { // double equals checks for equality
    colorMode = 1; // single equals is for assigning a new value to the variable
  }

  if (digitalRead(greenButton) == LOW) {
    colorMode = 2;
  }

  if (digitalRead(blueButton) == LOW) {
    colorMode = 3;
  }

  // read from the potentiometer, divide by 16 to get a number we can use for a color value
  potValue = analogRead(potentiometer) / 16;
  changeColor(colorMode, potValue); // call changeColor function

}

// changeColor takes a color mode and a potentiometer value and changes the color and brightness of the Photon RGB LED
void changeColor(int _colorMode, int _potValue) {

  if (_colorMode == 1) {
    RGB.color(_potValue, 0, 0);
  }

  if (_colorMode == 2) {
    RGB.color(0, _potValue, 0);
  }

  if (_colorMode == 3) {
    RGB.color(0, 0, _potValue);
  }

  else if (_colorMode == 0) {
    RGB.color(0, 0, 0);
  }
}
