/*  SparkFun Inventor's Kit for Photon
    Experiment 2 - Part 1: With a Touch of a Button
    This sketch was written by SparkFun Electronics
    August 31, 2015
    https://github.com/sparkfun

    This is a simple example sketch that turns on an LED
    when pushing down on the push button

    Development environment specifics:
    Particle Build environment (https://www.particle.io/build)
    Particle Photon RedBoard
    Released under the MIT License(http://opensource.org/licenses/MIT)
*/

int led = D0; // We name pin D0 as led
int pushButton = D2; // We name pin D2 as pushButton

// This routine runs only once upon reset
void setup() 
{
  pinMode(led, OUTPUT); // Initialize D0 pin as output
  pinMode(pushButton, INPUT_PULLUP); 
  // Initialize D2 pin as input with an internal pull-up resistor
}

// This routine loops forever
void loop() 
{
  int pushButtonState; 

  pushButtonState = digitalRead(pushButton);

  if(pushButtonState == LOW){ //If we push down on the push button
    digitalWrite(led, HIGH); //Turn ON the LED
  }
  else
  {
    digitalWrite(led, LOW); //Turn OFF the LED 
  }

}

