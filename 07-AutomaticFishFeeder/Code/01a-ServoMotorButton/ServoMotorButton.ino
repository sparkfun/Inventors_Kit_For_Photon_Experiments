/*  SparkFun Inventor's Kit for Photon
    Experiment 7 - Part 1a: Servo Motor 
    This sketch was written by SparkFun Electronics
    Joel Bartlett <joel@sparkfun.com>
    August 31, 2015
    https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments

    This application controls a servo with the press of a button. 
    Development environment specifics:
    Particle Build environment (https://www.particle.io/build)
    Particle Photon RedBoard
    Released under the MIT License(http://opensource.org/licenses/MIT)
*/
Servo myservo;// create servo object using the built-in Particle Servo Library

int button = D1;    //declare variable for button
int servoPin = D0;  //declare variable for servo
int pos = 0;        //variable to keep track of the servo's position
bool flag = 1;      //variable to keep track of the button presses

// This routine runs only once upon reset
void setup()
{
  Serial.begin(9600);//Start the Serial port @ 9600 baud

  pinMode(button, INPUT_PULLUP);   // sets button pin as input with internal pullup resistor


  myservo.attach(servoPin);  //Initialize the servo attached to pin D0
  myservo.write(180);        //set servo to furthest position
  delay(500);                //delay to give the servo time to move to its position
  myservo.detach();          //detach the servo to prevent it from jittering

}

// This routine loops forever
void loop()
{
    if(digitalRead(button) == LOW) //if a button press has been detected...
    {
      //This is known a s state machine.
      //It will move the servo to the opposite end from where it's set currently
      if(flag == 1)
        pos = 0;
      if(flag == 0)
        pos = 180;

      myservo.attach(servoPin);
      myservo.write(pos);
      delay(500);           //debounce and give servo time to move
      myservo.detach();

      flag = !flag;         //set flag to the opposite of what it's currently set to
      Serial.println(pos);  //prints to the serial port to keep track of the position
    }

}
