/*	SparkFun Inventor's Kit for Photon
        Experiment 11 - Part 1: Digital OLED Watch
        This sketch was written by SparkFun Electronics
	Jim Lindblom <jim@sparkfun.com>
	August 31, 2015
	https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments
	
	This application turns your Photon into a digital watch w/ OLED display
	It includes a 24-hour clock, stopwatch, and timer.
	All you have to do is find a way to strap it to your wrist...

	Development environment specifics:
	Particle Build environment (https://www.particle.io/build)
	Particle Photon RedBoard
	Released under the MIT License(http://opensource.org/licenses/MIT)
*/

// !!! REQUIRED LIBRARIES !!!
// Before trying to flash this app, include the SparkFunMicroOled library!
// https://github.com/sparkfun/SparkFun_Micro_OLED_Particle_Library

#include "math.h"

/////////////////
// Button Pins //
/////////////////
#define MODE_GREEN_PIN   D2
#define STOP_BLUE_PIN    D3
#define START_YELLOW_PIN D4

//////////////////////////
// MicroOLED Definition //
//////////////////////////
#define PIN_OLED_RST D6  // Connect RST to pin 6
#define PIN_OLED_DC  D5  // Connect DC to pin 5 (required for SPI)
#define PIN_OLED_CS  A2  // Connect CS to pin A2 (required for SPI)
MicroOLED oled(MODE_SPI, PIN_OLED_RST, PIN_OLED_DC, PIN_OLED_CS);

// Set your Time zone below, adjust from UTC.
// E.g. -6.0 is mountain time in the United Sates, 
// -4.0 would be eastern, -7.0 pacific
#define TIME_ZONE -6.0 // Mountain time (Niwot, CO; SparkFun HQ!)

////////////////////////////
// CLock Mode Definitions //
////////////////////////////
#define NUM_CLOCK_STATES 3 //total number of clock modes
// The clockStates enum defines our three clock modes, and their order
enum clockStates {
    MODE_CLOCK,     // Simple 24-hour clock
    MODE_STOPWATCH, // Stopwatch
    MODE_TIMER      // Egg timer
};
int clockState = MODE_CLOCK; // clockState keeps track of our current mode

/////////////////////////
// StopWatch Variables //
/////////////////////////
unsigned long stopWatchStart = 0; // Keeps track of the stopwatch start time
unsigned long stopWatchStop = 0; // Keeps track of the stopwatch stop time
bool stopWatchRunning = false; // Boolean so we know if the stopwatch is running or not
unsigned long runTime; // A varaible we'll use to calculate how long the stopwatch has been running

/////////////////////
// Timer Variables //
/////////////////////
bool settingTimer = true; // Boolean to track whether the timer is being set or not
unsigned long timerTime;  // Variable to keep track of when the timer ends

///////////////////////////////
// Button Debounce Variables //
///////////////////////////////
// debounceTime defines the number of milliseconds that must pass between
// button presses.
#define debounceTime 100 // 100ms debounce time
unsigned int lastPressTime = 0; // keeps track of the last time a button was pressed

// ISR: startWatch() -- called whenever the yellow button is pressed.
// This function starts the stopwatch, if it's in that mode.
// If it's in timer mode, pressing the yellow button will allow you to set the time.
void startWatch()
{
    if (softwareDebounce()) // If the button hasn't been pressed in debounceTime ms
    {
        if (clockState == MODE_STOPWATCH) // If we're in stopwatch mode
        {
            if (!stopWatchRunning) // And if the stopwatch is stopped
            {
                // Set the stopWatchRunning flag to true. The loop() will see that
                // next time through, and begin running the watch.
                stopWatchRunning = true;
                // Depending on whether the stopwatch has been paused or reset, we need to
                // modify the stopWatchStart value.
                if (stopWatchStop == stopWatchStart) // If the stopwatch has been reset
                    stopWatchStart = millis(); // stopwatch start time is just millis()
                else // If it was paused, but then re-started
                    stopWatchStart += millis() - stopWatchStop; // artificially increase start time by the amount paused
            }
            else
            {
                // Going Further: Add splits, pressing run while running starts a new lap
            }
        }
        else if (clockState == MODE_TIMER) // If we're in timer mode
        {
            // Both blue and yellow buttons need to be pressed to activate
            // setting timer mode
            noInterrupts(); // Turn off interrupts briefly
            if (digitalRead(STOP_BLUE_PIN) == LOW) // Check if the other button is pressed
                settingTimer = true; // Switch to setting the timer mode
            interrupts(); // re-enable interrupts
        }
    }
}

// ISR: stopWatch() -- called when the blue button is pressed.
// This function stops the stop watch, if it's running.
// If the stopwatch is not running, it resets the stopwatch.
void stopWatch()
{
    if (softwareDebounce()) // If the button hasn't been pressed in debounceTime ms
    {
        if (clockState == MODE_STOPWATCH) // And we're in stopwatch mode
        {
            if (stopWatchRunning) // If the stopwatch is runinng, we need to stop it
            {
                stopWatchStop = millis(); // Store the stop time
                stopWatchRunning = false; // Set the stopWatchRunning flag to false
                // Next time loop() looks at the at boolean, it'll stop the watch.
            }
            else // If already stopped, reset the stopwatch 
            {
                stopWatchStart = millis(); // Set the stopwatch start time to now
                stopWatchStop = stopWatchStart; // reset the stop time.
            }
        }
        else if (clockState == MODE_TIMER) // Otherwise, if we're in timer mode
        {
            // Both blue and yellow buttons need to be pressed to activate
            // setting timer mode
            noInterrupts(); // Turn off interrupts briefly
            if (digitalRead(START_YELLOW_PIN) == LOW) // Check if the other button is pressed
                settingTimer = true; // Switch to setting the timer mode
            interrupts(); // Re-enable interrupts
        }
    }
}

// ISR: changeMode is attached to the MODE_PIN interrupt. It is configured to be called
// any time the pin falls (when the button is pressed).
void changeMode()
{
    // softwareDebounce() helps "debounce" our button pin. When a button is pressed,
    // signal noise, and mechanical imperfections can make it rise and fall many times
    // before holding low.
    // softwareDebounce() filters out the high-frequency button-press noise, so the ISR
    // only runs once when the button is pressed.
    if (softwareDebounce())
    {
        // Increment clockState by one, then use the mod operation (%)
        // to roll back to 0 if we increment past a defined state.
        // E.g., NUM_CLOCK_STATES is 3, so clockState will go 0, 1, 2, 0, 1, 2, ...
        clockState = (clockState + 1) % NUM_CLOCK_STATES;
    }
}

void setup() 
{
    pinMode(MODE_GREEN_PIN, INPUT_PULLUP); // Setup the MODE_PIN as an input, with pull-up.
    // Since it has a pull-up, the MODE_PIN will be LOW when the button is pressed.
    // Attach an interrupt to MODE_PIN, `changeMode` will be the name of the ISR,
    // setting the event to RISING means the ISR will be triggered when the button is released
    attachInterrupt(MODE_GREEN_PIN, changeMode, RISING);
    
    pinMode(START_YELLOW_PIN, INPUT_PULLUP); // Set the yellow button pin as an input, with pull-up
    attachInterrupt(START_YELLOW_PIN, startWatch, FALLING); // Call startWatch ISR whenever the pin goes low
    
    pinMode(STOP_BLUE_PIN, INPUT_PULLUP); // Set the blue button as an input, with pull-up
    attachInterrupt(STOP_BLUE_PIN, stopWatch, FALLING); // Call stopWatch ISR whenever the button is pressed
    
    // Call the interrupts() function to enable interrupts.
    // The opposite of interrupts() is noInterrupts(), which disables external interrupts.
    interrupts();
    
    oled.begin(); // Initialize the OLED
    
    Time.zone(TIME_ZONE); // Set up the timezone
}

// loop() looks at the clockState variable to decied which digital watch
// function to draw on the screen.
void loop() 
{
    unsigned int h, m, s, ms; // Misc. variables used throughout the switch
    
    switch (clockState) // Check the value of clockState
    {
    case MODE_CLOCK: // If we're in clock mode:
        // Call the displayClock function (defined later), with the current
        // hour, minute, and second from the Time class.
        displayClock(Time.hour(), Time.minute(), Time.second());
        break;
    case MODE_STOPWATCH: // If we're in stopwatch mode:
        if (stopWatchRunning) // And if the stopwatch is running
        {
            // Calculate the most up-to-date run time:
            runTime = millis() - stopWatchStart;
        }
        else // If the stopwatch is stopped
        {
            // Calculate the total run time:
            runTime = stopWatchStop - stopWatchStart;
        }
        // runTime is the number of MILLISECONDS that the stopwatch has run
        // Let's calculate that out to  hours, minutes, seconds, and miliseconds
        h = runTime / (60 * 1000 * 60); // 3600000 milliseconds in an hour
        runTime -= h * (60 * 1000 * 60); // subtract the number of hour-milliseconds out
        m = runTime / (60 * 1000); // 60000 milliseconds in a minute
        runTime -= (60 * 1000) * m; // subtract the number of minute-milliseconds out
        s = runTime / 1000; // 1000 milliseconds in second
        runTime -= (1000 * s); // subtract the number of second-milliseconds
        ms = runTime; // We're left with the number of milliseconds
        // Call the displayStopwatch() function (defined later) to 
        // draw a stopwatch:
        displayStopWatch(h, m, s, ms);
        break;
    case MODE_TIMER: // Finally, if we're in timer mode
        if(settingTimer) // and if we're setting the timer
        {
            setTimer(); // Call setTimer() (defined below)
        }
        else // If the timer's already set, draw the time left
        {
            if (timerTime >= Time.now()) // If we haven't hit the alarm time yet
            {
                s = timerTime - Time.now(); // calculate the amount of seconds left in our timer
                h = (s / 60) / 60; // Use that value to find the number of hours left, if any
                m = (s / 60) % 60; // Then find the number of minutes left
                s = s % 60; // Then find the number of seconds left
                // Call the displayTimer() function, defined later to draw the timer.
                displayTimer(h, m, s);
            }
            else // If our timer has run out
            {
                displayAlert(); // Display an alarm!
            }
        }
        break;
    }
}

// displayClock draws the current hour and minute in a HH:MM format.
// Our large font doesn't leave room for seconds, so we'll use seconds to
// blink the colon.
void displayClock(unsigned int hours, unsigned int minutes, unsigned int seconds)
{
    oled.clear(PAGE); // Clear the display
    oled.setFontType(3); // Switch to the large-number font
    oled.setCursor(0, 0); // Set the cursor to top-left
    
    // printWithLeadZero adds as many '0''s as required to fill out a number's
    // digit count. E.g., if hours = 7, this will print "07"
    // The second parameter defines the number of digits a number should fill.
    printWithLeadZero(hours, 2); // Print two-characters for hours
    
    // Next the colon (:), or not.
    if ((seconds % 2) == 0) // If seconds is even
        oled.print(":");    // Print a colon
    else                    // Otherwise
        oled.print(" ");    // Print a space
        
    // Another printWithLeadZero, this time for minutes.
    printWithLeadZero(minutes, 2);
        
    oled.display(); // Update the display
}

// displayStopWatch draws the number of hours, minutes, seconds, and milliseconds
// passed since the stopwatch started.
void displayStopWatch(unsigned int hour, unsigned int min, unsigned int sec, unsigned int ms)
{
    oled.clear(PAGE); // Clear the display
    oled.setFontType(2); // Set the font to big, seven-segment-style numbers
    oled.setCursor(0, 0); // Set the cursor to top-left
    printWithLeadZero(hour, 2); // Print the hour, with an extra '0' if necessary
    oled.print('.'); // Print a '.' (':' isn't specified in this font)
    printWithLeadZero(min, 2); // Print the minute value
    
    // Now move the cursor down a little more than the next line.
    oled.setCursor(0, oled.getFontHeight() + 3);
    oled.print('.'); // Print another '.'
    printWithLeadZero(sec, 2); // Print seconds, with lead zero if necessary
    
    oled.setFontType(1); // Change the font to medium-size
    printWithLeadZero(ms, 3); // Print the number of milliseconds (this time with 3 digits)
    
    oled.display(); // Update the display
}

void setTimer()
{
    unsigned int hour = 0;  // variable to keep track of timer hours
    unsigned int minute = 0; // variable to keep track of timer minutes
    
    // We're not going to use interrupts this time. Just going to
    // poll for button presses
    // To disable interrupts, call the noInterrupts() function.
    noInterrupts(); // Disable interrupts
    
    // Wait for both blue and yellow buttons to be released
    while (!digitalRead(STOP_BLUE_PIN) || !digitalRead(START_YELLOW_PIN))
        Particle.process(); // Maintain the cloud connection
    
    // Loop forever, until the green button is pressed.
    while(digitalRead(MODE_GREEN_PIN))
    {
        // If the yellow button is pressed (allowing for the software debounce)
        if (!digitalRead(START_YELLOW_PIN) && softwareDebounce())
            hour = (hour + 1) % 99; // Increment the hour value, don't let it go above 99
        // If the blue button is pressed (plus debounce time)
        if (!digitalRead(STOP_BLUE_PIN) && softwareDebounce())
            minute = (minute + 1) % 60; // Increment the minute value (don't exceed 60)
        
        // Draw the time we're setting:
        oled.clear(PAGE); // Clear display
        oled.setFontType(1); // Medium size font
        oled.setCursor(0, 0); // top-left cursor
        oled.print("TimerSt"); // Print a different heading, to show it's the setting timer mode
        oled.line(0, oled.getFontHeight(), oled.getLCDWidth(), oled.getFontHeight());
        
        oled.setFontType(2); // Medium-size 7-segment display font
        oled.setCursor(0, (oled.getLCDHeight() / 2));
        printWithLeadZero(hour, 2); // Draw the hours, with leading 0
        oled.print(".");
        printWithLeadZero(minute, 2); // Draw the minutes, with a leading 0
        oled.display(); // Update the display
        
        Particle.process(); // Maintain cloud connection
    }
    // Wait for the green button to be released (go HIGH)
    while (digitalRead(MODE_GREEN_PIN) == LOW)
        Particle.process(); // Maintain the cloud connection
    
    // Calculate the timerTime, in seconds, based on current time (Time.now())
    timerTime = Time.now() + (hour * 60 * 60) + (minute * 60);
    
    // We may have entered this mode accidentally. If the timer hasn't been set
    // Don't start it.
    if (timerTime != Time.now())
        settingTimer = false; // remain in setting timer mode
    
    interrupts(); // re-enable interrupts
}

// If hour is greater than 0, then it'll draw HH:MM.
// If hour is 0, it'll draw MM:SS
void displayTimer(unsigned int hour, unsigned int min, unsigned int sec)
{
    unsigned int first, second; // variables to track first and second digit
    // Figure out if we need to draw HH:MM or MM:SS
    if (hour != 0)
    {
        first = hour;
        second = min;
    }
    else
    {
        first = min;
        second = sec;
    }
    
    oled.clear(PAGE); // Clear display
    
    oled.setFontType(1); // Medium font
    oled.setCursor(0, 0); // Top-left cursor
    oled.print(" Timer "); // Draw "timer" heading
    oled.line(0, oled.getFontHeight(), oled.getLCDWidth(), oled.getFontHeight()); // Then a line
    
    oled.setFontType(2); // Medium, 7-segment number font
    oled.setCursor(0, (oled.getLCDHeight() / 2)); // Set cursor below about the middle-left
    printWithLeadZero(first, 2); // Print the first number
    // If we're display HH:MM, we'll blink the decimal point every other second:
    if (((Time.second() % 2) == 0) || hour == 0)
        oled.print(".");
    else // If we're displaying MM:SS, always display the .
        oled.print(" ");
    printWithLeadZero(second, 2); // Print the second number
    oled.display(); // Update the display
}

// Display an alarm, if the timer has ended.
void displayAlert()
{
    oled.clear(PAGE);  // Clear display
    oled.setFontType(1); // Medium size font (7 columns, 3 rows)
    oled.setCursor(0, 0); // Top-left cursor
    oled.print("!!!!!!!");
    oled.print("!ALARM!");
    oled.print("!!!!!!!");
    oled.display(); // update display
}

// Prints as many lead zero's as necessary given a number 'n', and a 
// number of 'digits' it should fill out.
// E.g. printWithLeadZero(42, 3) will print "042"
// printWithLeadZero(7, 2); will print "07"
void printWithLeadZero(unsigned int n, int digits)
{
    for (int i=1; i<digits; i++) // Cycle through digit-1 times
    {
        if (n < (pow(10, i))) // If a number is less than 10^i
            oled.print("0"); // Print a leading zero
    }
    oled.print(n); // print the rest of the number
}

// softwareDebounce keeps our button from "bouncing" around. When a button is
// pressed, the signal tends to fluctuate rapidly between high and low.
// This function filters out high-frequency button presses, limiting
// them to debounceTime ms.
bool softwareDebounce()
{
    // If it's been at least debounceTime ms since the last press
    if (millis() > (debounceTime + lastPressTime))
    {
        lastPressTime = millis(); // update lastButtonPush
        return true;
    }
    // Otherwise return false.
    return false;
}
