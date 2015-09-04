/*  SparkFun Inventor's Kit for Photon
    Experiment 7 - Part 2: Automatic Fish Feeder
    This sketch was written by SparkFun Electronics
    Joel Bartlett <joel@sparkfun.com>
    August 31, 2015
    https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments

    This application controls a servo at a specific time and over the web. 
    Development environment specifics:
    Particle Build environment (https://www.particle.io/build)
    Particle Photon RedBoard
    Released under the MIT License(http://opensource.org/licenses/MIT)
*/
Serv
Servo myservo;// create servo object using the built-in Particle Servo Library

int servoPin = D0;  //declare variable for servo

int myMinute = 0;  //00
int myHour = 9; //9 am
int myDay = 14;
int myMonth = 8;//August


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

  // We are also going to declare a Spark.function so that we can trigger our feeder from the cloud.
  Spark.function("feed",triggerFeed);
  // When we ask the cloud for the function "feed", it will employ the function triggerFeed() from this app.

}

void loop()
{
  //The Photon uses 24 hour time so there's no confusion between 1am and 1pm, etc.
  //This time we are checking for a specific date and time.
  if(Time.month() == myMonth && Time.day() == myDay) //check the date, if it's not the right day, don't bother with the time.
  {
      if(Time.hour() == myHour && Time.minute() == myMinute) //check the time
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

}


int triggerFeed(String command)
{
/* Spark.functions always take a string as an argument and return an integer.
Since we can pass a string, it means that we can give the program commands on how the function should be used.

In this case, telling the function "feed" will trigger the servo.
Then, the function returns a value to us to let us know what happened.
In this case, it will return 1 if the function was called and -1 if we
received a totally bogus command that didn't do anything.
This does not check for any mechanical failures however. Only code.
*/

    if (command=="feed")
    {
        myservo.attach(servoPin);
        myservo.write(0);//set to a zero position. Dumps food
        delay(2000);
        myservo.write(180);//holding position
        delay(500);
        myservo.detach();
        return 1;
    }
    else
        return -1;
}
