###Introduction

This experiment will hook the Photon up to a [temperature/humidity](https://www.sparkfun.com/products/10167) sensor and a [photocell](https://www.sparkfun.com/products/9088) to observe lighting conditions. We'll initially use [serial communication](tutorials/8) to check their readings. Once you've gotten a handle on interacting with those sensors, we'll gather their data and regularly post it to [data.sparkfun.com](https://data.sparkfun.com), SparkFun's free data storage service.

In addition to covering the basics of serial communication, this experiment will also introduce **libraries**. Libraries are a powerful tool in the Particle IDE. They're pre-written files of code designed to accomplish certain tasks -- like reading a sensor -- in a very concise manner. They'll make your life a _lot_ easier.

Once the experiment is completed, you'll have a fully functional environmental data logging station, which can be observed from anywhere in the world!

####Parts Needed

* **1x** RHT03 Humidity and Temperature Sensor
* **1x** Mini Photocell
* **1x** 330&Omega; Resistor
* **7x** Jumper Wires

<!-- products_by_id(12795, 10167, 9088, 11507) -->

####Suggested Reading

* [Serial Communication](tutorials/8) -- Serial interfaces allow devices to exchange complex strings of data using just two wires. In this experiment, we'll use a serial interface between the Photon RedBoard and our computer to check the latest readings from the light and temperature/humidity sensors.
* [Serial Terminal Basics](tutorials/112) -- To interact with a serial interface on your computer, you'll need a **serial terminal program**. You may already have a serial terminal installed on your computer. If not, check out this tutorial for our recommendations, and a guide to getting started.
* [Including Particle Libraries](https://learn.sparkfun.com/tutorials/photon-development-guide/particle-build-online#build-libraries) -- Libraries are one of the most powerful tools availabe in the Particle Build IDE. Read through this tutorial to find out how to add a library to your application code.

###Hardware Hookup

Hook up your circuit as shown below:

-> [![Environment monitor fritzing diagram](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/environment-monitor-fritzing_bb.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/environment-monitor-fritzing_bb.png) <-

The yellow wire running from the photocell to the Photon's A0 pin carries an analog value relative to the light being sensed. The blue wire, running between the RHT03 sensor and Photon pin D3, is a very precisely-timed digital data interface. The SparkFunRHT03 library simplifies this interface to just a few lines of code.

Both the photocell and the RTH03 need a power supply to be operational. In this circuit, we're powering both off the Photon's 3.3V/GND supply.

###Photon Code

Copy and paste the code below into a new application -- ours is called **EnvironmentLoggerSerial**:

<pre style="height:500px;overflow:scroll;white-space:pre-wrap;white-space:-moz-pre-wrap;white-space:-pre-wrap;white-space:-o-pre-wrap;word-wrap:break-word;" data-language="language:c,c">
<code>
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
</code>
</pre>

But wait! Don't try to upload it yet. In fact, if you try to compile, you should [get an error](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/environment-monitor-build-error.png), because we need to **add the SparkFunRHT03 library**.

Click the Libraries icon ![library icon](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/environment-monitor-library-icon.png) on the left. Then click into the search bar and find **SparkFunRHT03**.

-> [![Searching for and including a library](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/environment-monitor-include-library.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/environment-monitor-include-library.png) <-

Next, click the "INCLUDE IN APP" button, and select the name of your new application, and verify by selecting "ADD TO THIS APP".

-> [![Adding a library to an app](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/environment-monitor-include-in-app.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/environment-monitor-include-in-app.png) <-

Two lines of code should be appended to your application: 

    language:c
    // This #include statement was automatically added by the Particle IDE.
    #include "SparkFunRHT03/SparkFunRHT03.h"

That's the library! Now you can verify and flash.

<div class="alert alert-warning"><b>Including libraries</b>: Unfortunately, there aren't any shortcuts to including libraries in your applications. Simply copying and pasting the <code>#include</code> statement won't <em>actually</em> include the library files in your application. Every time you want to include a library, you'll have to go through the process described above.</div>

###Code to Note

#### Serial

Among other things, this example introduces serial communication with functions like `Serial.begin()` and `Serial.print()`. To initialize a serial interface, call `Serial.begin([baud])` where `[baud]` sets the baud rate of the interface. In this example, we set the baud rate to 9600bps -- a reliable (if slow) standard rate -- in the `setup()` function:

	language:c
	void setup() 
	{
		...
		Serial.begin(9600); // Start the serial interface at 9600 bps
		...
	}

To send data _out_ of a serial interface, use either `Serial.print()`, `Serial.println()`, or `Serial.write()`. This example makes use of all three, for example:

	language:c
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

For more information on Serial functions, check out [Particle's reference documentation](https://docs.particle.io/reference/firmware/photon/#serial).

#### RHT03 Humidty and Temperature Sensor Library

Including the SparkFunRHT03 library gives us access to the RHT03 class. To begin using that class, we need to begin by creating an `RHT03` object in the global section:

	language:c
	RHT03 rht; // This creates a RTH03 object, which we'll use to interact with the sensor

`rht` is the object we'll use from here on to interact with the sensor. Once that's set up, we can call `rht.begin([DATA_PIN])` in the `setup()` function to initialize the sensor. `DATA_PIN` should be the Photon digital pin connected to the sensor.

	language:c
	const int RHT03_DATA_PIN = D3; // RHT03 data pin
	...
	void setup()
	{
		...
		rht.begin(RHT03_DATA_PIN); // Initialize the RHT03 sensor
		...
	}

Finally, we can read the the temperature and humidity values from the sensor. This requires two steps: updating all values and reading them. Begin by calling `rht.update()`. If `update()` succeeds (indicated by returning a `1`), you can call `rht.tempF()`, `rht.tempC()`, and `rht.humidity()` to get the values of interest:

	language:c
	int update = rht.update(); // Read from the RHT
	if (update == 1) // If the update was successful:
	{
		float humidity = rht.humidity(); // Read humidity into a variable
		float tempC = rht.tempC(); // Read celsius temperature into a variable
		float tempF = rht.tempF(); // Read farenheit temperature into a variable
		...
	}

###What You Should See

After uploading the code, open up a serial terminal to your Photon RedBoard's port. If you're on a Windows machine, it should look something like "COMN". On Mac or Linux, the port should be something like "/dev/tty.usbmodenNNNN".

Make sure the baud rate is set to 9600. Once the Photon connects (starts pulsing cyan), you should begin to see the light, temperature, and humidity values stream by.

-> [![Example serial monitor output](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/environment-monitor-serial.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/environment-monitor-serial.png) <-

Cover up the light sensor, or shine a flashlight on it. Covering the sensor should make the readings go down -- can you get it to 0?

Breathe on the temperature/humidity sensor. Does the temperature value go up? Can you influence the humidity value (don't try too hard -- no pouring water on the sensor!).

###Troubleshooting

If your terminal program won't let you open the serial port -- citing an error like: "COM port in use". Or, if the COM port has disappeared from a selectable list. Try following these steps:

1. Close the serial terminal program.
2. Reset your Photon RedBoard
3. Wait for the RedBoard to connect to WiFi
4. Open the terminal program back up and try connecting.

If all you see is a stream of "Error reading from the RHT03", make sure everything is correctly wired up. If you're still not having any luck with the sensor, consider getting in touch with our [tech support team](!!!). (It's normal for the "Error..." output to show up occasionally -- the RHT03's one-wire interface isn't the most robust one out there.)

##Part 2: Posting Environment Data to Data.SparkFun.com

As you may have already experienced, using the serial port on a Photon can be very cumbersome. Let's come up with some way to show off the sensor readings without having to fuss with terminal programs. SparkFun's [Data.SparkFun.com](https://data.sparkfun.com) service is a great, FREE, utility for posting, storing, and reading sets of data.

To get the most out of this example, you should create a data.sparkfun.com data stream of your own. Go to [data.sparkfun.com/streams/make](https://data.sparkfun.com/streams/make), then fill out the form to look something like this:

-> [![Example Phant stream creation](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/environment-monitor-phant-create.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/environment-monitor-phant-create.png) <-

The most critical box is the "Fields" list. Make sure those four strings are typed exactly as **humidity**, **light**, **tempc**, and **tempf**. After creating the stream, you'll be given a set of keys -- long strings of random characters. Don't lose the keys! Either copy them down, or use the "Email me my keys" feature.

###New Photon Code

Create a new application, or paste this code over the previous example.

<pre style="height:500px;overflow:scroll;white-space:pre-wrap;white-space:-moz-pre-wrap;white-space:-pre-wrap;white-space:-o-pre-wrap;word-wrap:break-word;" data-language="language:c,c">
<code>
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

///////////////////////////////////////////////
// Phant (data.sparkfun.com) Keys and Server //
///////////////////////////////////////////////
// These keys are given to you when you create a new stream:
const char server[] = "data.sparkfun.com"; // Phant destination server
const char publicKey[] = "LQLwdljGJ2TqXgxvAlGv"; // Phant public key
const char privateKey[] = "A1x5rDzw47HWPqyog0lo"; // Phant private key
Phant phant(server, publicKey, privateKey); // Create a Phant object

///////////////////////
// Post Rate Control //
///////////////////////
// data.sparkfun.com limits how often you can post to the service. You are allowed up
// to 100 posts every 15 minutes.
unsigned long lastPost = 0; // lastPost keeps track of the last UNIX time we posted
const unsigned int POST_RATE_S = 60; // This sets the post rate to 60 seconds. Avoid setting it below 10s.

//////////////////////////
// Station Name Globals //
//////////////////////////
String stationName = ""; // String object to keep track of our Photon's name

void setup() 
{
    Serial.begin(9600); // Start the serial interface at 9600 bps
    
    rht.begin(RHT03_DATA_PIN); // Initialize the RHT03 sensor
    
    pinMode(LIGHT_PIN, INPUT); // Set the photocell pin as an INPUT.
    pinMode(LED_PIN, OUTPUT); // Set the LED pin as an OUTPUT
    digitalWrite(LED_PIN, LOW); // Initially set the LED pin low -- turn the LED off.
    
    // getDeviceName() -- defined at the bottom of this code -- polls Particle's
    // server to get the name of the Photon running this application.
    getDeviceName(); // Update the stationName String
}

void loop() 
{
    // This conditional should only run when the last successful post to Phant
    // was POST_RATE_S (60 seconds) or longer ago.
    // Time.now() returns the current UNIX timestamp (number of seconds since January 1, 1970).
    // It should increase by 1 every second. On a successful POST, we set lastPost equal to Time.now().
    if (lastPost + POST_RATE_S <= Time.now())
    {
        digitalWrite(LED_PIN, HIGH); // Turn the LED on to indicate we're posting
        int update = rht.update(); // Get new values from the RHT03.
        
        if (update == 1) // If the RHT03 update was successful
        {
    		int postResult = 0; // This variable will keep track of whether or not
            
            //while (postResult <= 0)
            // Phant posts aren't always successful. Our postToPhant() function,
            // defined below, will return 1 if it was successful. Or a negative
            // number if it failed.
            while (postToPhant() <= 0)
            {
                Serial.println("Phant post failed. Trying again."); // Debug statement
                delay(1000); // Delay 1s so we don't flood the server
            }
            // After a successful Phant POST:
            Serial.println("Phant post success!"); // Debug print
            // Set lastPost to current time, so we don't post for another POST_RATE_S seconds:
            lastPost = Time.now(); 
        }
        else // If the RHT03 update failed:
        {
            delay(RHT_READ_INTERVAL_MS); // Delay to give the sensor time to reset
        }
        digitalWrite(LED_PIN, LOW); // Turn the LED off to indicate we're done posting (/trying to post)
    }
}

// postToPhant() gathers all of our sensor data, bundles it into a Phant post,
// and sends it out to data.sparkfun.com.
// It'll return either 1 on success, or a negative number if the post fails
int postToPhant(void)
{
    Serial.println("Posting to Phant!"); // Debug statement
    
	// Use phant.add(<field>, <value>) to add data to each field.
	// Phant requires you to update each and every field before posting,
	// make sure all fields defined in the stream are added here.
    phant.add("humidity", rht.humidity(), 1); // These first three phant adds set a field value to float variable
    phant.add("tempc", rht.tempC(), 1); // The third parameter -- valid for float variables -- sets the number
    phant.add("tempf", rht.tempF(), 1); // of decimal points after the number.
    phant.add("light", analogRead(LIGHT_PIN)); // We can also add an integer
    phant.add("station", stationName); // phant.add(<field>, <String>) is perfectly valid too!
    
    // phant.particlePost() performs all of the Phant server connection and HTTP POSTING for you.
    // It'll either return a 1 on success or negative number on fail.
    // It uses the field/value combinations added previously to send Phant its data.
    // MAKE SURE YOU COMMIT A phant.add() FOR EVERY FIELD IN YOUR STREAM BEFORE POSTING!
    return phant.particlePost();
}

///////////////////////////////
// Get Device Name Functions //
///////////////////////////////
// These sets of functions poll Particle's server for the name of our Photon.
// This method is described in Particle's documentation here:
// https://docs.particle.io/reference/firmware/photon/#get-device-name
// Function handlers are used here -- when the Spark.subscribe function
// returns, it will call the nameHandler([topic], [data]) function with our 
// Photon's name.
bool validName = false; // Boolean to track if we have a valid name or not

// nameHandler() is a function handler. It's passed to Spark.subscribe() and
// called when that function returns.
// The [data] variable should have the name of our Photon when it's done.
void nameHandler(const char *topic, const char *data)
{
    stationName = String(data);  // Store the name in the stationName global variable
    validName = true; // Set validName to true, so getDeviceName can stop blocking.
}

// getDeviceName manages the spark subscribing and publishing required to get our
// Photon's name. It'll block for up to 30 seconds. On success it'll return a
// positive number. On fail, it'll return 0.
int getDeviceName(void)
{
    Spark.subscribe("spark/", nameHandler);
    Spark.publish("spark/device/name");
    
    int timeout = 30;
    while ((!validName) && (timeout > 0))
    {
        Serial.println("Waiting for name..." + String(timeout--));
        delay(1000); // Spark.process() is called during delay()
    }
    
    Serial.println("Station name = " + stationName);
    
    return timeout;
}
</code>
</pre>

But wait! Again! Time to include another library. This time search for the **SparkFunPhant** library, and include it in your app. If you created a new application, you'll also need to re-include the **SparkFunRHT03** library.

-> [![include the SparkFunPhant library](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/environment-monitor-phant-library-include.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/environment-monitor-phant-library-include.png) <-

If you created a Phant stream of your own, paste your **public and private** keys into the `publicKey[]` and `privateKey[]` variable values. If you haven't created your own stream yet, feel free to take ours for a test drive. You can find the stream [here](https://data.sparkfun.com/streams/LQLwdljGJ2TqXgxvAlGv). Just please, please don't abuse it. Don't set your post rate to faster than 60 seconds, and, once you've verified it works, try creating a stream of your own.

###Code to Note

This application introduces the Phant library, a useful tool to make posting to Phant as painless as can be. To set the Phant library up in your sketch, begin by declaring your keys and the Phant server (data.sparkfun.com):

	language:c
	const char server[] = "data.sparkfun.com"; // Phant destination server
	const char publicKey[] = "LQLwdljGJ2TqXgxvAlGv"; // Phant public key
	const char privateKey[] = "A1x5rDzw47HWPqyog0lo"; // Phant private key
	Phant phant(server, publicKey, privateKey); // Create a Phant object

The last line there creates a `Phant` object -- `phant` (lowercase 'p') -- which we'll use throughout the rest of the application to send data to Phant. The constructor call gives the Phant object almost everything it needs to know about your Phant stream: the server and the public and private keys.

Posting data to Phant is a two step process: adding field/data combinations, and posting them to the server. To add data call the `phant.add(<field>, <value>)` function where `<field>` is a String matching exactly one of the fields in your stream. The `<value>` part of the Phant post can be almost any data type: in our example we're adding `float`'s, `int`s, and a `String`.

The entire Phant post takes place in our `int postToPhant(void)` function:

	language:c
	int postToPhant(void)
	{
		phant.add("humidity", rht.humidity(), 1); // These first three phant adds set a field value to float variable
		phant.add("tempc", rht.tempC(), 1); // The third parameter -- valid for float variables -- sets the number
		phant.add("tempf", rht.tempF(), 1); // of decimal points after the number.
		phant.add("light", analogRead(LIGHT_PIN)); // We can also add an integer
		phant.add("station", stationName); // phant.add(<field>, <String>) is perfectly valid too!
		
		return phant.particlePost(); // Post our values to the Phant server
	}

After you've commited an `add` for **every field** in your Phant stream, call `phant.particlePost()` to send them out to the server. This function takes care of the TCP connection, and sends out a valid HTTP POST to the Phant server. When it's done, it'll either return a `1` on success, or yield a negative number if it fails.

###What You Should See

After uploading the code, keep an eye on your Photon's blue D7 LED. It should illuminate when the device is attempting to send data to data.sparkfun.com, and it'll turn off when it's done. To check if your data has been posted to Phant, load up the stream in your web browser (click [here](https://data.sparkfun.com/streams/LQLwdljGJ2TqXgxvAlGv) if you're using our test stream). You should see a new row of data at the top, stamped with your Photon's name, data, and a recent timestamp.

-> [![Example screenshot of data.sparkfun.com](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/environment-monitor-phant-screenshot.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/environment-monitor-phant-screenshot.png) <-

Every minute you should see a new row of data appear. Let it run all night! See how cold your room gets.

Now that your data is on data.sparkfun.com, you can have all sorts of fun with it. For instance, you can load your public key into [analog.io](https://analog.io) -- our friends who've created a beautiful front end graphing interface for Phant. Or you can try to create graphs of your own -- check out [this tutorial using Google Charts](http://phant.io/graphing/google/2014/07/07/graphing-data/).

###Troubleshooting

If you're not seeing any data from your Photon being posted to the stream, it may be failing on the `phant.particlePost()` function. There are a few reasons the Phant POST might fail:

* No Internet connection -- make sure your Photon is pulsing cyan.
* Invalid POST -- make sure you have a `phant.add` for every field in your stream. Also make sure there aren't any typos in the field names.
* Stream overload -- Phant streams on data.sparkfun.com are limited to 300 posts every 15 minutes. Our shared stream is subject to that same limitation. Consider making a stream of your own. It's free!

We still use serial in this example, but really only for debugging. If you're still not having any luck, open up your serial port and see what it has to say.
