/*  SparkFun Inventor's Kit for Photon
    Experiment 1 - Part 1: Hello World Blink an LED
    This sketch was written by SparkFun Electronics
    August 31, 2015
    https://github.com/sparkfun

    This is a simple example sketch that turns on an LED 
    for one second, off for one second, and repeats forever.

    Development environment specifics:
    Particle Build environment (https://www.particle.io/build)
    Particle Photon RedBoard
    Released under the MIT License(http://opensource.org/licenses/MIT)
*/

int led = D0; // We name pin D0 as led

// This routine runs only once upon reset
void setup()
{
  pinMode(led, OUTPUT); // Initialize D0 pin as output
}

// This routine loops forever
void loop()
{
  digitalWrite(led, HIGH);  // Turn ON the LED
  delay(1000);              // Wait for 1000mS = 1 second
  digitalWrite(led, LOW);   // Turn OFF the LED
  delay(1000);              // Wait for 1 second
}

