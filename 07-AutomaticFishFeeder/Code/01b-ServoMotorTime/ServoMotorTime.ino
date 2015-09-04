/*  SparkFun Inventor's Kit for Photon
	Experiment 7 - Part 1b: Servo Motor with Time
	This sketch was written by SparkFun Electronics
	Joel Bartlett <joel@sparkfun.com>
	August 31, 2015
	https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments

	This application controls a servo at a specific time. 
	Development environment specifics:
	Particle Build environment (https://www.particle.io/build)
	Particle Photon RedBoard
	Released under the MIT License(http://opensource.org/licenses/MIT)
*/
Servo myservo;// create servo object using the built-in Particle Servo Library

int servoPin = D0;  //declare variable for servo

void setup()
{
  
  myservo.attach(servoPin);  //Initialize the servo attached to pin D0
  myservo.write(180);        //set servo to 180. This position will hold the food
  delay(500);                //delay to give the servo time to move to its position
  myservo.detach();          //detach the servo to prevent it from jittering
  
  Time.zone(-6);//Set timezone to Mountain Daylight Time (MDT) Spring/Summer
  //Time.zone(-7);//Set timezone to Mountain Standard Time (MST) Fall/Winter
  //Find out your time zone here: http://www.timeanddate.com/time/map/
  
  Spark.syncTime();//sync with the Particle Cloud's time

}

void loop()
{
  //This if statement checks to see if it is the correct hour and minute of the day to dispense food.
  //The Photon uses 24 hour time so there's no confusion between 1am and 1pm, etc.
  if(Time.hour() == 15 && Time.minute() == 0)  //feed at 3:00pm
  {
	myservo.attach(servoPin);
	myservo.write(0);//set to a zero position. Dumps food
	delay(2000);
	myservo.write(180);//set to a zero position
	delay(500);
	myservo.detach();//detach to keep the servo from jittering
	
	delay(60000);//wait the rest of the minute out
  }

}
