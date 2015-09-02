/*	SparkFun Inventor's Kit for Photon
        Experiment 8 - Part 1: Accelerometer Serial Streamer
        This sketch was written by SparkFun Electronics
	Jim Lindblom <jim@sparkfun.com>
	August 31, 2015
	https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments
	
	This application gathers data from an MMA8452Q and prints
	it out over the Photon's USB serial port.

	Development environment specifics:
	Particle Build environment (https://www.particle.io/build)
	Particle Photon RedBoard
	Released under the MIT License(http://opensource.org/licenses/MIT)
*/

// !!! REQUIRED LIBRARIES !!!
// Before trying to flash this app, include the SparkFunMMA8452Q library!
// https://github.com/sparkfun/SparkFun_MMA8452Q_Particle_Library

// Create an MMA8452Q object, used throughout the rest of the sketch.
MMA8452Q accel; // Default constructor, SA0 pin is HIGH

// The above works if the MMA8452Q's address select pin (SA0) is high.
// If SA0 is low (if the jumper on the back of the SparkFun MMA8452Q breakout
// board is closed), initialize it like this:
// MMA8452Q accel(MMA8452Q_ADD_SA0_); 

void setup() 
{
    Serial.begin(9600);
    
	// Initialize the accelerometer with begin():
	// begin can take two parameters: full-scale range, and output data rate (ODR).
	// Full-scale range can be: SCALE_2G, SCALE_4G, or SCALE_8G (2, 4, or 8g)
	// ODR can be: ODR_800, ODR_400, ODR_200, ODR_100, ODR_50, ODR_12, ODR_6 or ODR_1
    accel.begin(SCALE_2G, ODR_1); // Set up accel with +/-2g range, and slowest (1Hz) ODR
}

void loop() 
{
	// accel.available() will return 1 if new data is available, 0 otherwise
    if (accel.available())
    {
		// To update acceleration values from the accelerometer, call accel.read();
        accel.read();
		
		// After reading, six class variables are updated: x, y, z, cx, cy, and cz.
		// Those are the raw, 12-bit values (x, y, and z) and the calculated
		// acceleration's in units of g (cx, cy, and cz).
		
		// use the printAccelGraph funciton to print the values along with a bar
		// graph, to see their relation to eachother:
        printAccelGraph(accel.cx, "X", 20, 2.0);
        printAccelGraph(accel.cy, "Y", 20, 2.0);
        printAccelGraph(accel.cz, "Z", 20, 2.0);
        Serial.println();
    }
	
	// No need to delay, since our ODR is set to 1Hz, accel.available() will only return 1
	// about once per second.
}

// printAccelGraph prints a simple ASCII bar graph for a single accelerometer axis value.
// Examples: 
//	printAccelGraph(-0.1, "X", 20, 2.0) will print:
// 		X:                    =|                     (0.1 g)
//	printAccelGraph(1.0, "Z", 20, 2.0) will print:
//		Z:                     |==========           (1.0 g)
// Input:
//	- [value]: calculated value of an accelerometer axis (e.g accel.cx, accel.cy)
//	- [name]: name of the axis (e.g. "X", "Y", "Z")
//	- [numBarsFull]: Maximum number of bars either right or left of 0 point.
//	- [rangeAbs]: Absolute value of the maximum acceleration range
void printAccelGraph(float value, String name, int numBarsFull, float rangeAbs)
{
	// Calculate the number of bars to fill, ignoring the sign of numBars for now.
	int numBars = abs(value / (rangeAbs / numBarsFull));
    
    Serial.print(name + ": "); // Print the axis name and a colon:
	
	// Do the negative half of the graph first:
    for (int i=0; i<numBarsFull; i++)
    {
        if (value < 0) // If the value is negative
        {
			// If our position in the graph is in the range we want to graph
            if (i >= (numBarsFull - numBars))
                Serial.print('='); // Print an '='
            else
                Serial.print(' '); // print spaces otherwise
        }
        else // If our value is positive, just print spaces
            Serial.print(' ');
    }
    
    Serial.print('|'); // Print a pipe (|) to represent the 0-point
    
	// Do the positive half of the graph last:
    for (int i=0; i<numBarsFull; i++)
    {
        if (value > 0)
        {	// If our position in the graph is in the range we want to graph
            if (i <= numBars)
                Serial.print('='); // Print an '='
            else
                Serial.print(' '); // otherwise print spaces
        }
        else // If value is negative, just print spaces
            Serial.print(' ');
    }
    
	// To end the line, print the actual value:
    Serial.println(" (" + String(value, 2) + " g)");
}