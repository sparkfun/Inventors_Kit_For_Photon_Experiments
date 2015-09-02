/*	SparkFun Inventor's Kit for Photon
        Experiment 8 - Part 2: Accelerometer Activity Publisher
        This sketch was written by SparkFun Electronics
	Jim Lindblom <jim@sparkfun.com>
	August 31, 2015
	https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments
	
	This application monitor's an MMA8452Q's pulse detection feature to
	count steps. When commanded to, it'll publish the step count to
	the Particle cloud.

	Development environment specifics:
	Particle Build environment (https://www.particle.io/build)
	Particle Photon RedBoard
	Released under the MIT License(http://opensource.org/licenses/MIT)
*/

// !!! REQUIRED LIBRARIES !!!
// Before trying to flash this app, include the SparkFunMMA8452Q library!
// https://github.com/sparkfun/SparkFun_MMA8452Q_Particle_Library

// Pin definitions:
const int BUTTON_PIN = 3; // The Publish button is connected to D3
const int LED_PIN = 7; // The on-board LED is used to indicate status

MMA8452Q accel; // Create an accelerometer object to be used throught the sketch

// stepCount keeps track of the number of steps since the last publish:
unsigned int stepCount = 0;

// To save battery power, we'll put the Photon in SEMI_AUTOMATIC mode.
// So our Photon will not attempt to connect to the Cloud automatically.
// It'll be up to us to issue Particle.connect() commands, when we want
// to connect.
// More on modes here: https://docs.particle.io/reference/firmware/photon/#system-modes
SYSTEM_MODE(SEMI_AUTOMATIC); // Set system mode to SEMI_AUTOMATIC

// We'll use a few booleans to keep track of _if_ we need to publish, 
// and if our publish was successful.
bool publishFlag = false;
bool publishSuccess = false;

void setup() 
{
    // Begin by turning WiFi off - we're running off batteries, so 
    // need to save as much power as possible.
    WiFi.off(); // Turn WiFi off
    
    // Configure our button and LED pins
    pinMode(LED_PIN, OUTPUT); // LED pin is set as an OUTPUT
    digitalWrite(LED_PIN, HIGH); // Write LOW to set LED off to begin
    pinMode(BUTTON_PIN, INPUT_PULLUP); // Button is configured as input w/ pull-up active
    
    // Initialize our accelerometer. Set the scale to high-resolution (2g)
    // Set the output data rate to 50Hz.
    accel.begin(SCALE_2G, ODR_50);
    
    // Next, we'll configure our accelerometer's tap detection interface. This is a 
    // very tweak-able function. We can configure the Threshold, time limit, and latency:
    
    // Pulse threshold: the threshold which is used by the system to detect
    // the start and end of a pulse.
    // Threshold can range from 1-127, with steps of 0.063g/bit.
    //byte threshold = 3; // 3 * 0.063g = 0.189g // This might work better in some cases
    byte threshold = 1; // 2 * 0.063g = 0.063g
    
    // Pulse time limit: Maximum time interval that can elapse between the start of
    // the acceleration exceeding the threshold, and the end, when acceleration goes
    // below the threshold to be considered a valid pulse.
    byte pulseTimeLimit = 255; // 0.625 * 255 = 159ms (max)
    
    // Pulse latency: the time interval that starts after first pulse detection, during
    // which all other pulses are ignored. (Debounces the pulses).
    // @50Hz: Each bit adds 10ms (max: 2.56s)
    byte pulseLatency = 64; // 1.25 * 64 = 640ms
    
    // Use the setupTap function to configure tap detection in our accelerometer:
    accel.setupTap(threshold, threshold, threshold, pulseTimeLimit, pulseLatency);
}

void loop() 
{
    // Use the readTap() function to check if a tap was detected:
    byte tap = accel.readTap();
    // readTap will return 1 if there was a tap
    if (tap != 0) // If there was a tap
    {
        stepCount++; // Increment stepCount
        toggleLED(); // Toggle the LED state
    }
    
    // Next, check if the button was pressed (it's active-low)
    if (digitalRead(BUTTON_PIN) == LOW)
    {   // If the button was pressed
        if (publishFlag == false) // and if we aren't publishing:
        {
            // We're in SEMI_AUTOMATIC system mode, so it's up to us
            // to connecto to the Particle cloud.
            // We've also turned WiFi off, so we'll have to connect to that too.
            WiFi.on();  // Turn WiFi on
            WiFi.connect(); // Connect to our WiFi network
            Particle.connect(); // Connect to the Particle cloud
            
            digitalWrite(LED_PIN, LOW); // Turn the LED off
            publishFlag = true; // Indicate that we are publishing
        }
    }
    
    // It takes a few seconds to connect to the Particle cloud.
    // We'll use Particle.connected() to check if we've connected
    // If we're connected, and need to publish:
    if (publishFlag && Particle.connected())
    {
        // Each step usually creates about 2 taps (arm or leg going forward then backward)
        stepCount /= 2; // Dividing by 2 usually makes for a much more accurate step count:
        
        // Call Particle.publish to push our step count to the web:
        publishSuccess = Particle.publish("Steps", String(stepCount / 2));
        
        // If the publish was successful
        if (publishSuccess)
        {
            publishFlag = false; // clear the publishFlag
            stepCount = 0; // and reset the step count
        }
        // Allow the Photon some time to Publish before disconnecting
        delay(5000); // Delay 5 seconds
    }
    
    // If we've successfully published, and are still connected to the Partice cloud
    if (publishSuccess && Particle.connected())
    {
        WiFi.off(); // Turn off WiFi
        publishSuccess = false; // Clear our publishSuccess flag
    }
}

// toggleLED() simply toggles the LED each time it's called. From on to off, or off to on.
void toggleLED()
{
    static bool ledState = true;
    if (ledState)
    {
        digitalWrite(LED_PIN, HIGH);
        ledState = false;
    }
    else
    {
        digitalWrite(LED_PIN, LOW);
        ledState = true;
    }
}