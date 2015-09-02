/*	SparkFun Inventor's Kit for Photon
        Experiment 6 - Part 1: Environment Logger Serial
        This sketch was written by SparkFun Electronics
	Jim Lindblom <jim@sparkfun.com>
	August 31, 2015
	https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments
	
	This application gathers data from a Photocell and an RHT03
	humidity and temperature sensor.
	The data is streamed over the Photon's USB serial port.

	Development environment specifics:
	Particle Build environment (https://www.particle.io/build)
	Particle Photon RedBoard
	Released under the MIT License(http://opensource.org/licenses/MIT)
*/

// !!! REQUIRED LIBRARIES !!!
// Before trying to flash this app, include the SparkFunRHT03 library!
// https://github.com/sparkfun/SparkFun_RHT03_Particle_Library

/////////////////////
// Pin Definitions //
/////////////////////
const int RHT03_DATA_PIN = D3; // RHT03 data pin
const int LIGHT_PIN = A0; // Photocell analog output
const int LED_PIN = D7; // LED to show when the sensor's are being read

///////////////////////////
// RHT03 Object Creation //
///////////////////////////
RHT03 rht; // This creates a RTH03 object, which we'll use to interact with the sensor

unsigned int minimumLight = 65536;
unsigned int maximumLight = 0;
float minimumTempC = 5505;
float maximumTempC = 0;
float minimumTempF = 9941;
float maximumTempF = 0;
float minimumHumidity = 100;
float maximumHumidity = 0;

#define PRINT_RATE 1500 // Time in ms to delay between prints.

void setup() 
{
    // Serial.begin() is used to open up a serial interface between the Photon
    // and your computer.
    // The '9600' parameter configures the speed of the interface. This value is
    // called the "baud rate", which is equivalent to bits per second (bps).
    Serial.begin(9600); // Start the serial interface at 9600 bps
    
    // Using the 'rht' object created in the global section, we'll begin by calling
    // its member function `begin`.
    // The parameter in this function is the DIGITAL PIN we're using to communicate
    // with the sensor.
    rht.begin(RHT03_DATA_PIN);  // Initialize the RHT03 sensor
    
    // Don't forget to set the pin modes of our analog sensor (INPUT) and the LED (OUTPUT):
    pinMode(LIGHT_PIN, INPUT); // Set the photocell pin as an INPUT.
    pinMode(LED_PIN, OUTPUT); // Set the LED pin as an OUTPUT
    digitalWrite(LED_PIN, LOW); // Initially set the LED pin low -- turn the LED off.
}

void loop() 
{
    digitalWrite(LED_PIN, HIGH); // Turn the LED on -- it'll blink whenever the sensor is being read.
    
    // Use the RHT03 member function `update()` to read new humidity and temperature values from the sensor.
    // There's a chance the reading might fail, so `update()` returns a success indicator. It'll return 1
    // if the update is successful, or a negative number if it fails.
    int update = rht.update();
    
    if (update == 1) // If the update succeeded, print out the new readings:
    {
        // Use analogRead to get the latest light sensor reading:
        unsigned int light = analogRead(LIGHT_PIN);
        // Do some math to calculate the minimum and maximum light sensor readings we've seen:
        if (light > maximumLight) maximumLight = light;
        if (light < minimumLight) minimumLight = light;
        
        // The `humidity()` RHT03 member function returns the last successfully read relative
        // humidity value from the RHT03.
        // It'll return a float value -- a percentage of RH between 0-100.
        // ONLY CALL THIS FUNCTION AFTER SUCCESSFULLY RUNNING rht.update()!.
        float humidity = rht.humidity();
        // Do some math to calculate the max/min humidity
        if (humidity > maximumHumidity) maximumHumidity = humidity;
        if (humidity < minimumHumidity) minimumHumidity = humidity;
        
        // The `tempF()` RHT03 member function returns the last succesfully read 
        // farenheit temperature value from the RHT03.
        // It returns a float variable equal to the temperature in Farenheit.
        // ONLY CALL THIS FUNCTION AFTER SUCCESSFULLY RUNNING rht.update()!.
        float tempF = rht.tempF();
        // Do some math to calculate the max/min tempF
        if (tempF > maximumTempF) maximumTempF = tempF;
        if (tempF < minimumTempF) minimumTempF = tempF;
        
        // `tempC()` works just like `tempF()`, but it returns the temperature value in
        // Celsius.
        // ONLY CALL THIS FUNCTION AFTER SUCCESSFULLY RUNNING rht.update()!.
        float tempC = rht.tempC();
        // Do some math to calculate the max/min tempC
        if (tempC > maximumTempC) maximumTempC = tempC;
        if (tempC < minimumTempC) minimumTempC = tempC;
        
        
        // `Serial.print()` is used to send data from the Photon to our computer over the serial interface.
        // The parameter passed to `print()` can be anything from a String, to a constant array of charaters,
        // to a float value, integer, or nearly any other type of variable.
        Serial.print("Light: "); // Print "Light: "
        Serial.print(light); // Print the light reading
        Serial.print(" ("); // Print " ("
        Serial.print(minimumLight); // Print the minimum light reading
        Serial.print('/'); // Print a '/' -- single quotes mean we're only sending one character
        Serial.print(maximumLight); // Print the maximum light reading.
        Serial.println(") (min/max)"); // Finish the line by printing ") (min/max)"
        // The full line will look something like: "Light: 545 (8/791) (min/max)"
        
        // Print the temperature in °C:
        // Example printout: "Temp: 24.9 °C (23.5/24.5) (min/max)"
        Serial.print("Temp: ");
        Serial.print(tempC, 1); // Printing a float, we can set the precision (number of decimal points) with the second parameter 
        Serial.print(" ");
        // `write()` can be used to write a SINGLE BYTE value over the serial line:
        Serial.write(248); // 248 is the ASCII value for the ° symbol. We're fancy.
        Serial.print("C (");
        Serial.print(minimumTempC, 1);
        Serial.print('/'); // Print a '/'
        Serial.print(maximumTempC, 1);
        Serial.println(") (min/max)");
        
        // Print the temperature in °F:
        // Example printout: "Temp: 76.1 °F (74.3/76.1) (min/max)"
        Serial.print("Temp: "); // Print "Temp: "
        Serial.print(tempF, 1); // Print the tempF variable -- 1 decimal point
        Serial.print(' ');      // Print a space
        Serial.write(248);      // Print ASCII value 248 (the ° symbol)
        Serial.print("F (");    // Print "F ("
        Serial.print(minimumTempF, 1); // Print minimum temperature -- 1 decimal point
        Serial.print('/');      // Print a '/'
        Serial.print(maximumTempF, 1); // Print maximum temp -- 1 decimal point
        Serial.println(") (min/max)"); // Finsh the line by printing ") (min/max)"
        
        // Print the relative humidity:
        // Example printout: Humidity: 29.7 % (29.10/41.80) (min/max)
        Serial.print("Humidity: ");
        Serial.print(humidity, 1);
        Serial.print(" %");
        Serial.print(" (");
        Serial.print(minimumHumidity, 1);
        Serial.print("/");
        Serial.print(maximumHumidity, 1);
        Serial.println(") (min/max)");
        
        Serial.println(); // Print a blank line:
    }
    else // If the update failed, give the sensor time to reset:
    {
        Serial.println("Error reading from the RHT03."); // Print an error message
        Serial.println(); // Print a blank line
        
        delay(RHT_READ_INTERVAL_MS); // The RHT03 needs about 1s between read attempts
    }
    digitalWrite(LED_PIN, LOW); // Turn the LED off
    
    delay(PRINT_RATE); // delay for 1s, printing too much will make the output very hard to read.
}
