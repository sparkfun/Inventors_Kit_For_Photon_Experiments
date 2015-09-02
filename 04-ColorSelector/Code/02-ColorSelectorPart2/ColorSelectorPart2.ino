/*  SparkFun Inventor's Kit for Photon
        Experiment 4 - Part 2
        This sketch was written by SparkFun Electronics
      Ben Leduc-Mills
      August 31, 2015
      https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments

      This is an example sketch showing how to change the color of the Photon Redboard onboard LED using Twitter and IFTTT (If this then that).

      Development environment specifics:
      Particle Build environment (https://www.particle.io/build)
      Particle Photon RedBoard
      Released under the MIT License(http://opensource.org/licenses/MIT)
 */
 
//declare the name of our function (and its parameters) at the top of our program
int rgbColor(String val);

//variables for our colors (red, green, blue)
int r, g, b;


void setup() {
  //take control of the Photon RGB LED
  RGB.control(true);
  //register our function in the Particle cloud
  Particle.function("rgbColor", rgbColor);
}

void loop() {
  //don't need to do anything in the loop
}

//our actual function call
//looking for a string of three numbers that represents an RGB color
//e.g. 200,12,42
int rgbColor(String val) {

  //check if incoming string is empty
  if (val.length() > 0) {

    //if not, use indexOf to find the first comma delimiter
    //this string class has no split command
    //more about indexOf: https://docs.particle.io/reference/firmware/photon/#indexof-
    int i = val.indexOf(",");

    //use substring to get the value from the beginning of the string until the first comma
    //then use toInt to convert from a string to an integer
    //which gets us our first number, the r value
    r = val.substring(0, i).toInt();

    //more string manipulation to get our g and b values
    int j = val.indexOf(",", i + 1);

    g = val.substring(i + 1, j).toInt();

    b = val.substring(j + 1, val.length()).toInt();

    //put it all together and make the LED light up
    RGB.color(r, g, b);

    //if we're successful return 1
    return 1;
  }
  //something went wrong
  else return -1;
}
