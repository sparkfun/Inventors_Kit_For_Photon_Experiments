###Introduction

This experiment will re-visit a hardware component you should already be familiar with: buttons. But the method we use to interact with the buttons in this experiment will be wholly different. We're going to use hardware interrupts to be instantly notified if a button is pressed.

Combining the buttons with an OLED -- user input with user interface -- allows us to create an endless variety of interactive applications. In this experiment -- using hardware interrupts as a foundation -- we'll create a simple clock, complete with stopwatch and timer. Then -- once you're comfortable with external interrupts -- we'll connect the same circuit to the Internet to create a fully functional weather forecasting application. You'll never have to listen to that weather reporter again!

####Parts Needed

* **1x** MicroOLED Breakout
* **3x** Buttons: green, yellow & blue
* **15x** Jumper Wires

<!-- products_by_id(12795, 13003, 10302) -->

####Suggested Reading

**External interrupts** are used in these experiments to read the state of our buttons. An interrupt is a program flow-control trick microprocessors use to execute a routine outside the `loop()`,  whenever a pin's digital state changes.

Implementing an interrupt requires three parameters:

* **Pin** -- Most of the Photon RedBoard's pins can be used to generate an interrupt. All of them except D0 and A5.
* **Event** -- When a digital pin changes state it either **rises** (goes from HIGH to LOW) or **falls** (goes from LOW to HIGH). An interrupt can be set to trigger on either of those events or both.
* **Interrupt routine** -- An interrupt service routine (**ISR**) is a function defined in your code -- with no parameters, and no return value -- that immediately runs whenever an interrupt event triggers.

Interrupts can trigger at almost any moment in the run cycle of an application, whether it's between a couple lines of code in your `loop()`, or during a function call. Even a `delay()` function can be paused by an interrupt. Once an interrupt has fired, and its ISR has run its course, your application code will continue running from where it was interrupted.

###Hardware Hookup

Hook up your circuit as shown below (if you still have the OLED hooked up from the last experiment, don't change a thing there, just add the buttons):

-> [![OLED Apps circuit diagram](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/oled-weather-fritzing_bb.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/oled-weather-fritzing_bb.png) <-

Most of that wire-jungle is devoted to the Micro OLED's [SPI interface](tutorials/16). It's critical that the three hardware SPI pins -- SCK/A3, MISO/A4, and MOSI/A5 -- not be relocated.

Our three button signals are all routed to Photon pins capable of handling external interrupts. We'll configure these pins with **internal pull-up resistors**, so when the button is pressed down, the pin will read as LOW.

###Photon Code

Create a new application -- we call ours **DigitalWatch** -- and paste the code below into it:

    language:c
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
            oled.print(" ");    // Print a spaces
            
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
    
    // If hour is 0, then it'll draw HH:MM.
    // If hour is greater than 0, it'll draw MM:SS
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

Before uploading the application, change the `TIME_ZONE` variable towards the top of the application to your location's time zone, adjusted from UTC.

#### Include the SparkFunMicroOLED Library

To use the Micro OLED display, we'll take advantage of the **SparkFunMicroOLED** library. Click over to the "Libraries", search for `SparkFunMicroOLED`, and click **INCLUDE IN APP**.

-> [![Including the SparkFunMicroOLED library](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/oled-includ-library.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/oled-includ-library.png) <-

Then click on your new _DigitalWatch_ application, and click **ADD TO THIS APP**. An `#include "SparkFunMicroOLED/SparkFunMicroOLED.h"` line should be added to the top of your application.

###Code to Note

#### Using External Interrupts

There are three external interrupts used in this application -- one for each button. Implementing an interrupt is a two-step process: attaching the interrupt to a pin, and creating an ISR. To attach the interrupt, call the [attachInterrupt()](https://docs.particle.io/reference/firmware/photon/#attachinterrupt-) function, which takes three parameters:

* **Pin number**: The pin number being monitored for state change.
* **ISR name**: The specific name of the interrupt function to be called when an interrupt ocurrs.
* **Event**: The pin-change event, which can be `RISING`, `FALLING`, or `CHANGE`. If set to change, the ISR will trigger whenever the pin state rises or falls.

For example, to attach an interrupt on the green, mode-changing pin, we call this `attachInterrupt()` function:

	language:c
	setup()
	{
		...
		pinMode(MODE_GREEN_PIN, INPUT_PULLUP); // Setup the MODE_PIN as an input, with pull-up.
        // Since it has a pull-up, the MODE_PIN will be LOW when the button is pressed.
        // Attach an interrupt to MODE_PIN, `changeMode` will be the name of the ISR,
        // setting the event to RISING means the ISR will be triggered when the button is released
        attachInterrupt(MODE_GREEN_PIN, changeMode, RISING);
        
        pinMode(START_YELLOW_PIN, INPUT_PULLUP); // Set the yellow button pin as an input, with pull-up
        attachInterrupt(START_YELLOW_PIN, startWatch, FALLING); // Call startWatch ISR whenever the pin goes low
		...
	}

Then, outside the `setup()` and `loop()` functions, we create an ISR with the same name as that in the ISR, `changeMode`.

	language:c
	// changeMode is attached to the MODE_PIN interrupt. It is configured to be called
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

Note that our ISR doesn't have any parameters, and it doesn't return anything. Instead we usually use global variables to have our interrupts control the application.

### What You Should See

The application begins in our digital clock state. Not much to see here, aside from the colon blinking every second. The current time is fetched from the `Time.hour()`, `Time.minute()` and `Time.second()` functions, so it should be well-synced with the Internet clock.

**Press the green button** to switch to the next clock display: a **stopwatch**. In this mode, the yellow button starts the counter, and the blue button stops and resets it.

Clicking mode again will set the clock into **timer** mode. Increment the number of hours by clicking the yellow button, and increment minutes by clicking blue. When your timer is all set, click the green button to start it. Now what this clock is _really_ missing is a buzzer. Try adding one from [experiment 5](https://learn.sparkfun.com/tutorials/sparkfun-inventors-kit-for-photon-experiment-guide/experiment-5-music-time).

To set the timer, press both the yellow and blue buttons simultaneously to enter time-setting mode again.

###Troubleshooting

If the buttons aren't behaving as you'd like them to -- whether they're too sensitive, or not sensitive enough -- you can try modifying the `debounceTime` variable near the top of the code. This sets the amount of milliseconds that must pass between button triggers.

Most of the time-keeping mechanisms in our clock are cloud-based. That means they're very accurate, but won't work if the Photon RedBoard isn't connected to the Internet. If your Photon RedBoard's LED isn't pulsing cyan, that may be the reason your clock isn't working (the dangers of Internet-ing everything!).

##Part 2: Weather Forecasting Application

Now that you know a thing-or-two about hardware interrupts -- and you've got a working OLED and button circuit -- time to take it to the Internet! We've got components wired up to combine a user-interface (UI) with user-input, let's use them to create a Weather Forecast App.

This experiment uses the [OpenWeatherMap](http://openweathermap.org/) [API](http://openweathermap.org/api), culling the amazing weather database for weather forecasts for anywhere in the world. With a simple HTTP GET request, we can find out the temperature, humidity, pressure, precipitation, wind, and other weather data, then display that data on the Micro OLED. And that weather data can be a current forecast, or future (hourly or daily) guesstimate.

### Pre-Experiment Setup

To customize your experiment, there are a few pieces of data you'll need to gather and stuff in your code.

#### Find Your City's Geographic Coordinates

To predict the weather in your city, you'll need to know its geographic coordinates. Don't know your city's latitude and longitude offhand? Use [Google Maps](https://support.google.com/maps/answer/18539?source=gsearch). Here's how:

1. Go to [Google Maps](https://www.google.com/maps/)
2. Search, scroll, or navigate to your location
3. Right click, select "What's here?"
4. Check out the info card under the search box
	
-> [![Grabbing the lat and lon from google maps](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/oled-weather-google-map.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/oled-weather-google-map.png) <-

-> _If you're curious what the weather at SparkFun HQ is, you can use lat, long = 40.090554, -105.184861._ <-

#### Get an OpenWeatherMap API Key

To gather data from OpenWeatherMap, you'll need an API key -- a unique hashed string that identifies you. The key is [free](http://openweathermap.org/price), but it limits you to **1,200 API calls per minute**. Don't spam their server!

Getting an OpenWeatherMap API key is simple:

1. Register on the [sign up page](http://home.openweathermap.org/users/sign_up)
	* You'll need to create a username/password, and supply your email address
2. Find the API key under setup.

-> [![Getting an OpenWeatherMap API Key](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/oled-open-weather-api-key.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/oled-open-weather-api-key.png) <-

Currently the experiment _does_ work without an API key, but it's best to go about this on the up-and-up.

### New Photon Code

Here's the main application code. Create a new application -- we've titled ours "oled-weatherapp" -- and paste this in.

    language:c
    ////////////////////////////
	// Button Pin Definitions //
	////////////////////////////
	#define GREEN_BUTTON  D2 // Green button switches through display pages
	#define BLUE_BUTTON   D3 // Blue button updates future forecast
	#define YELLOW_BUTTON D4 // Yellow butotn updates current forecast

	//////////////////////////
	// MicroOLED Definition //
	//////////////////////////
	#define PIN_OLED_RST D6 // Connect RST to pin 7 (req. for SPI and I2C)
	#define PIN_OLED_DC  D5 // Connect DC to pin 3 (required for SPI)
	#define PIN_OLED_CS  A2 // Connect CS to pin A2 (required for SPI)
	MicroOLED oled(MODE_SPI, PIN_OLED_RST, PIN_OLED_DC, PIN_OLED_CS);

	//////////////////////////////
	// OpenWeatherMap Variables //
	//////////////////////////////
	// OpenWeathermap API key. Get one (for free) by signing up here:
	//                   http://home.openweathermap.org/users/sign_up
	const String OPENWEATHER_API_KEY = ""; 
	// Forecast location declaration (currently set for Niwot, CO - SparkFun HQ!):
	const float LATITUDE = 40.090554; // Weather forecast location's latitude
	const float LONGITUDE = -105.184861; // Weather forecast location's longitude
	// Create an OpenWeather object, giving it our API key as the parameter:
	OpenWeather weather(OPENWEATHER_API_KEY);

	///////////////////////////
	// Future Forecast Order //
	///////////////////////////
	// These variables control how many future forecasts our app will do, and their order.
	const int FUTURE_FORECAST_NUM = 6; // Number of future forecasts
	// forecastOrder defines the order of forecasts. Each value is the number of
	// hours in the future to forecast. It should be multiples of 3. Once you get
	// to 24, it should be multiples of 24.
	const int forecastOrder[FUTURE_FORECAST_NUM] = {3, 6, 9, 24, 48, 72};
	// forecastOrderIndex keeps track of our position in the forecastOrder array
	volatile int forecastOrderIndex = FUTURE_FORECAST_NUM - 1;

	///////////////////////////////
	// Display Mode Page Control //
	///////////////////////////////
	// This enum defines all of our display modes and their order.
	enum t_displayModes {
		DISPLAY_BIG_3,                    // avg temperature, humidity, and pressure
		DISPLAY_TEMPERATURES,             // min, max, and avg temperatures
		DISPLAY_WIND,                     // any wind conditions, speed, and direction.
		DISPLAY_PRECIPITATION,            // precipitation (if any)
		DISPLAY_WEATHER_CONDITION_NAME,   // descripton of the weather
		DISPLAY_WEATHER_CONDITION_SYMBOL, // a graphic for the weather
		DISPLAY_UPDATE_TIME               // the forecast's time
	};
	const int NUM_DISPLAY_MODES = 7; // Number of values defined in enum above
	volatile int displayMode = NUM_DISPLAY_MODES - 1; // Keeps track of current display page
	#define DISPLAY_UPDATE_RATE 10 // Cycle display every 10 seconds
	int lastDisplayUpdate = 0; // Stores time of last display update

	/////////////////////
	// Interrupt Flags //
	/////////////////////
	// These boolean flags help us keep track of what needs to happen after
	// we execute an ISR.
	volatile bool doUpdateWeather = true; // If true, loop should update the current weather
	volatile bool doFutureUpdate = false; // If true, loop should update future weather
	volatile bool doUpdateDisplay = false; // If true, loop should update the display

	///////////////////////////////
	// Software Debounce Control //
	///////////////////////////////
	unsigned long lastButtonPush = 0; // Stores the time of the last button press
	const int BUTTON_DEBOUNCE_TIME = 200; // Minimum time (in ms) between button presses

	// ISR: updateWeather - sets a flag to have loop() get the current weather
	// forecast. Also prints a message to the OLED to indicate as much.
	void updateWeather()
	{
		// softwareDebounce() (defined at the bottom of the application code)
		// makes sure button presses can only occur every BUTTON_DEBOUNCE_TIME ms.
		if (softwareDebounce()) 
		{
			oled.clear(PAGE); // Clear the display
			oled.setFontType(0); // Smallest font
			oled.setCursor(0, 0); // Cursor to top-left
			// multiLinePrint takes a long string, and tries to break it into multiple
			// lines - split by spaces in the string.
			multiLinePrint("Updating Current Weather!");
			oled.display(); // Update the display
			
			// Now set doUpdateWather to true, so our loop() knows to get the 
			// current weather update next time it runs through.
			doUpdateWeather = true;
			// Play with displayMode's value so the app doesn't skip to the
			// next page.
			displayMode = (displayMode + FUTURE_FORECAST_NUM - 1) % FUTURE_FORECAST_NUM;
		}
	}

	// ISR: updateFuture - sets a flag to have loop() get a future forecast
	// Also prints a message to the OLED to indicate as much.
	void updateFuture()
	{
		if (softwareDebounce()) // If it's been BUTTON_DEBOUNCE_TIME since the last press
		{
		  
			forecastOrderIndex = (forecastOrderIndex + 1) % FUTURE_FORECAST_NUM;
			
			oled.clear(PAGE); // Clear the display
			oled.setFontType(0); // Set font to smallest type
			oled.setCursor(0, 0); // Set cursor to top-left
			multiLinePrint("Updating Future Forecast!"); // Print the message
			oled.println(); // Print a blank line
			// Now let's print the future time in hours or days of the forecast:
			if (forecastOrder[forecastOrderIndex] < 24) // Less than 24 hours?
				oled.println(String(forecastOrder[forecastOrderIndex]) + " hrs"); // Print # of hours
			else // Otherwise
				oled.println(String(forecastOrder[forecastOrderIndex] / 24) + " day(s)"); // Print number of days
			oled.display(); // Update the display
			
			// Now set doFutureUpdate to true, so our loop() knows to get the 
			// future weather update next time it runs through.
			doFutureUpdate = true;
			// Play with displayMode's value so the app doesn't skip to the
			// next page.
			displayMode = (displayMode + FUTURE_FORECAST_NUM - 1) % FUTURE_FORECAST_NUM;
		}
	}

	// ISR: forwardDisplayMode - cycles through pages in the display.
	void forwardDisplayMode()
	{
		if (softwareDebounce()) // If it's been BUTTON_DEBOUNCE_TIME since the last press
		{
			// set doDisplayUpdate flag, so loop() knows to update the display
			// next time through.
			doUpdateDisplay = true;
		}
	}

	void setup()
	{
		// Set up our button pin modes, and tie them to interrupts.
		// For all of these buttons, set the interrupt event to RISING.
		// Since they're active-low, that'll mean they trigger when
		// the button is released.
		// YELLOW_BUTTON: updates the current weather forecast
		pinMode(YELLOW_BUTTON, INPUT_PULLUP);
		attachInterrupt(YELLOW_BUTTON, updateWeather, RISING);
		// BLUE_BUTTON: updates a future weather forecast
		pinMode(BLUE_BUTTON, INPUT_PULLUP);
		attachInterrupt(BLUE_BUTTON, updateFuture, RISING);
		// GREEN_BUTTON: cycles our display to the next page
		pinMode(GREEN_BUTTON, INPUT_PULLUP);
		attachInterrupt(GREEN_BUTTON, forwardDisplayMode, RISING);
		
		// OpenWeatherMap configuration: set the units to either IMPERIAL or METRIC
		// Which will set temperature values to either farenheit or celsius
		weather.setUnits(IMPERIAL); // Set temperature units to farenheit
		
		// Display set up:
		oled.begin(); // Initialize the display.
		oled.clear(PAGE); // Clear the display
		oled.setFontType(0); // Set font to smallest type
		oled.setCursor(0, oled.getLCDHeight()/2); // Set font cursor to middle-left
		oled.println("Connecting"); // Display the start-up message
		oled.display(); // Update the display
	}

	// loop() is mostly just a series of flag checks. If a flag was set by an interrupt,
	// loop() has to take an action, make sure to clear the flag afterwards.
	void loop()
	{
		// If the yellow button is pressed and released, doUpdateWeather should be
		// true. If it is, we'll try to update the weather.
		if (doUpdateWeather)
		{
			// Use the OpenWeather class's current(<lat>, <lon>) function to 
			// update the current weather forecast.
			// If current() succeeds, it'll return 1, and update a number of
			// get function return values we can then use.
			if (weather.current(LATITUDE, LONGITUDE))
			{
				// Clear the doUpdateWeather flag, so this doesn't run again
				// until the button is pressed.
				doUpdateWeather = false;
				// Set the doUpdateDisplay flag, so loop() updates the display
				// when it gets to that point.
				doUpdateDisplay = true;
			}
			else
			{   // If the weather update fails
				delay(1000); // Delay 1 second
				// Dont' clear the doUpdateWeather flag, so we'll try again
			}
		}
		
		// If the blue button is pressed and released, doFutureUpdate should be
		// true. If it is, we'll try to update the future forecast.
		if (doFutureUpdate)
		{
			// Future forecasts can either be daily or hourly. If our position in
			// the forecastOrder array points to a number less than 24 hours do an hourly forecast
			if (forecastOrder[forecastOrderIndex] < 24)
			{
				// To do an hourly forecast use the hourly(<lat>, <lon>, <hours/3>) function.
				// We can only forecast in 3-hour intervals. So, for example, to forecast out
				// 9 hours, call hourly(LATITUDE, LONGITUDE, 3); where 3 = 9/3
				if (weather.hourly(LATITUDE, LONGITUDE, (forecastOrder[forecastOrderIndex] / 3) + 1))
				{
					// If the hourly forecast succeeds
					doFutureUpdate = false; // Clear the doFutureUpdate flag
					doUpdateDisplay = true; // Set the doUpdateDisplay flag
				}
				else
				{
					// If the hourly forecast failed
					delay(1000); // Delay 1 second, then try again next time through the loop()
				}
			}
			else
			{
				// Daily forecasts can be scraped using the daily(<lat>, <lon>, <days - 1>) funciton.
				// If you want today's daily forecast (different from current) call daily(LATITUDE, LONGITUDE, 1);
				// For tomorrow's forecast, call daily(LATITUDE, LONGITUDE, 2), etc.
				// Since forecastOrder points to a number of hours, divide them by 24 to get the days
				if (weather.daily(LATITUDE, LONGITUDE, (forecastOrder[forecastOrderIndex] / 24) + 1))
				{
					// If the daily forecast succeeds:
					doFutureUpdate = false; // Clear the doFutureUpdate flag
					doUpdateDisplay = true; // Set the doUpdateDisplay flag
				}
				else
				{
					// If the daily update fails
					delay(1000); // delay for 1 second, then try again.
				}
			}
		}
		
		// There are two cases where we need to update the app's display:
		//  1. If the doUpdateDisplay was set by an interrupt or other function.
		//  2. Every DISPLAY_UPDATE_RATE seconds, the display will cycle itself
		if ((Time.now() >= (lastDisplayUpdate + DISPLAY_UPDATE_RATE)) || doUpdateDisplay)
		{
			// Increment displayMode, next time through a new page will be displayed:
			displayMode = (displayMode + 1) % NUM_DISPLAY_MODES;
			
			updateWeatherDisplay(); // This function draws the active weather data page
			
			// Update lastDisplayTime, so we don't come back for DISPLAY_UPDATE_RATE seconds
			lastDisplayUpdate = Time.now();
			doUpdateDisplay = false; // Clear the doUpdateDisplay flag, in case that's what triggerd
		}
		
		displayProgressBar(); // Draws a progress bar at the bottom of the screen
	}

	// updateWeatherDisplay uses the displayMode variable to draw one of our weather
	// apps pages of data.
	void updateWeatherDisplay()
	{
		
		oled.clear(PAGE); // clear the display, our display functions won't
		
		switch (displayMode)
		{
		case DISPLAY_BIG_3: // Display temperature, humidity, and pressure
			displayBig3();
			break;
		case DISPLAY_TEMPERATURES: // Displays average, min, and max temps
			displayTemperatures();
			break;
		case DISPLAY_WIND: // Displays any wind characteristics
			displayWind();
			break;
		case DISPLAY_PRECIPITATION: // Displays any rain/snow
			displayPrecipitation();
			break;
		case DISPLAY_WEATHER_CONDITION_NAME: // Displays weather conditions
			displayWeatherConditionName();
			break;
		case DISPLAY_WEATHER_CONDITION_SYMBOL: // Displays weather picture
			displayWeatherConditionSymbol();
			break;
		case DISPLAY_UPDATE_TIME: // Displays the forecast update time
			displayUpdateTime();
			break;
		}
		
		oled.display(); // Actually draw the display (our display functions won't)
	}

	void displayBig3()
	{
		oled.setCursor(0, 0); // Set the cursor to top-left
		oled.setFontType(1); // Set font size to "medium"
		// To get the latest temperature, humidity, and pressure values
		// from our OpenWeather class. Use the temperature(), humidity(),
		// and pressure() functions.
		oled.println(String(weather.temperature(), 1) + " F"); // Print temperature
		oled.println(String(weather.humidity()) + "% RH"); // Print humidity
		oled.println(String((unsigned int) weather.pressure()) + "hPa"); // Print pressure
	}

	void displayTemperatures()
	{
		// printHeading is defined toward the bottom of the application code.
		// It prints a title with a line under it. It returns the vertical cursor position
		int cursorY = printHeading("   Temps  "); // Print Temps, with a line under it
		
		oled.setFontType(0); // set font size to smallest
		oled.setCursor(0, cursorY); // Set cursor for the rest of our text
		
		// The average, maximum, and minimum forecast temperatures can be retreived
		// using the temperature(), maxTemperature(), and minTemperature() functions.
		// All three return float values.
		oled.print("Avg: " + String(weather.temperature(), 1)); // Print average temperature
		
		cursorY += oled.getFontHeight() + 1;
		oled.setCursor(0, cursorY); // Move the cursor a little more than one character-height down
		oled.print("Max: " + String(weather.maxTemperature(), 1)); // Print maximum temperature
		
		cursorY += oled.getFontHeight() + 1;
		oled.setCursor(0, cursorY); // Move the cursor a little more than one character-height down
		oled.print("Min: " + String(weather.minTemperature(), 1)); // Print minimum temperature
	}

	void displayWind()
	{
		int cursorY = printHeading("   Wind   "); // Print the "Wind" heading
		
		oled.setFontType(0); // Smallest font
		oled.setCursor(0, cursorY); // Use return from printHeading to set our cursor Y position
		
		// The wind forecast includes wind speed (in meters per second), direction, and a 
		// descriptive name (like "light breeze"). To get those values call either:
		// - windSpeed() -- a float variable, e.g. 3.14
		// - windDirection() -- a String variable, e.g. "W", or "SE"
		// - windName() -- a String variable, e.g. "light breeze"
		
		// Print wind speed and compass direction
		oled.print(String(weather.windSpeed(), 2) + ' ' + weather.windDirection());
		
		cursorY += oled.getFontHeight() + 3;
		oled.setCursor(0, cursorY); // Move cursor down font height + 3.
		multiLinePrint(weather.windName()); // Print the wind's description
	}

	void displayPrecipitation()
	{
		int cursorY = printHeading("  Precip  "); // Print "precip" heading
		
		oled.setFontType(1); // Medium-size font
		oled.setCursor(0, cursorY); // Set cursor based on return from printHeading
		
		// To get the precipitation forecast, call precipitationType(), which may return
		// "Rain", "Snow", or NULL if there is not precipitation in the forecast
		// If there is precipitation, you can find the amount by calling
		// precipitationValue(), which returns a the amount of precipitation in mm (?)
		if (weather.precipitationType() == NULL)
		{
			// If there is no precipitation forecasted, print "None"
			oled.print(" None  ");
		}
		else
		{
			// If precip is forecasted, use multiLinePrint to print the type
			// of precip (rain, snow, etc.)
			multiLinePrint(weather.precipitationType());
			// Then print the amount forecasted:
			oled.print(String(weather.precipitationValue(), 2) + "mm");
		}
	}

	void displayWeatherConditionName()
	{
		int cursorY = printHeading("Conditions"); // Print "conditions" heading.
		
		oled.setFontType(0); // Smallest font
		oled.setCursor(0, cursorY); // Cursor just below heading line
		
		// Weather conditions, like "light rain", "haze", "broken clouds", etc.
		// can be retrieved using the conditionName() function, which returns a String
		multiLinePrint(weather.conditionName()); // Print condition name broken up by space
	}

	// Draw bitmap based on weather condition code.
	// More info here: http://openweathermap.org/weather-conditions
	void displayWeatherConditionSymbol()
	{
		// There are tons of weather condition codes defined by OpenWeatherMap. Listed here:
		// http://openweathermap.org/weather-conditions
		// E.g. 211=thunderstorm, 781=tornado, 800=clear sky
		// Groupings of weather ID's all map to a certain picture.
		// use the conditionID() function to get the weather code, an integer
		int weatherCode = weather.conditionID();
		
		// Then map that integer to a weather symbol to be drawn
		if ((weatherCode >= 200) && (weatherCode <= 232)) // Thunderstorm
			oled.drawBitmap(bmp_thunderstorm);
		else if ((weatherCode >= 300) && (weatherCode <= 321)) // Drizzle
			oled.drawBitmap(bmp_shower_rain);
		else if ((weatherCode >= 500) && (weatherCode <= 531)) // Rain
			oled.drawBitmap(bmp_shower_rain);
		else if ((weatherCode >= 600) && (weatherCode <= 622)) // Snow
			oled.drawBitmap(bmp_snow);
		else if ((weatherCode >= 701) && (weatherCode <= 781)) // Atmosphere
			oled.drawBitmap(bmp_haze);
		else if (weatherCode == 800) // Clear sky
			oled.drawBitmap(bmp_clear_sky);
		else if (weatherCode == 801) // Few clouds
			oled.drawBitmap(bmp_few_clouds);
		else if (weatherCode == 802) // Scattered clouds
			oled.drawBitmap(bmp_scattered_clouds);
		else if ((weatherCode == 803) || (weatherCode == 804)) // Broken or overcast
			oled.drawBitmap(bmp_broken_clouds);
		else
		{
			//! TODO: could be tornado, tropical storm, hurricane, cold, hot, windy
			// hail, calm, breezy, gales, storms.
			// Use the LCDAssistant program to make more bitmaps.
		}
	}

	void displayUpdateTime()
	{
		int cursorY = printHeading(" Time/Day "); // Print time/day heading
		
		oled.setFontType(0); // Smallest font
		oled.setCursor(0, cursorY); // Cursor just below the heading line
		
		// The day of a weather forecast can be retreived using the getDate()
		// function, which is a string.
		// It'll be in the formware of YYYY-MM-DD, e.g. 2015-09-04
		// 10 characters! perfect for the 0 font.
		oled.print(weather.getDate());  // Print the date
		
		cursorY += oled.getFontHeight() + 3;
		oled.setCursor(0, cursorY); // Move text cursor down a little extra
		
		// The time of a weather forecast is available with the getTime() funciton.
		// This function also returns a String, in the format of HH:MM:SS,
		// e.g. 18:42:00
		oled.println(weather.getTime()); // Print the forecast time
	}

	// This is a handy function that takes a long-ish string, finds the spaces
	// and prints each word line-by-line.
	// It's not smart enough to print two small words on the same line, but it could be!
	void multiLinePrint(String s)
	{
		String split = s; // We'll continut to split the split String until it's just one word
		
		// Loop until the our split String is small enough to fit on a single line.
		// Divide the LCD's pixel width (64) by the width of a character+1 to get
		// characters per line.
		while (split.length() >= (oled.getLCDWidth() / (oled.getFontWidth() + 1)))
		{
			// Use the indexOf String function to find a space (' ').
			int space = split.indexOf(' ');
			if (space > 0) // If there is a space, indexOf will return it's position.
			{
				// print from the beginning of our split string to the character
				// before the space.
				oled.println(split.substring(0, space));
			}
			else // Otherwise, if there is no space, it'll return a negative number
			{    // If there are no spaces left
				break;  // break out of the while loop
			}
			// Shorten up the split string, get rid of everything we've printed, 
			// plus the space. Then loop back through.
			split = split.substring(space + 1);
		}
		// Print the last bit of the split string.
		oled.println(split);
	}

	// Print heading takes a string, prints it out at the top of the display,
	// then prints a line under that string.
	// It'll return the y-cursor position 3 pixels under the line.
	int printHeading(String heading)
	{
		oled.setFontType(0); // Smallest font type
		oled.setCursor(0, 0); // Top-left cursor position
		oled.print(heading); // Print the heading text
		
		int vPos = oled.getFontHeight() + 3; // Calculate the y-value of our line
		oled.line(0, vPos, oled.getLCDWidth(), vPos); // Draw the line horizontally
		
		return vPos + 3; // Return the y-cursor position
	}

	// This function draws a line at the very bottom of the screen showing how long
	// it'll be before the screen updates.
	void displayProgressBar()
	{
		// Use lastDisplayUpdate's time, the current time (Time.now()) and the 
		// total time per page (DISPLAY_UPDATE_RATE) to calculate what portion
		// of the display bar needs to be drawn.
		float percentage = (float)(Time.now() + 1 - lastDisplayUpdate) / (float)DISPLAY_UPDATE_RATE;
		// Mutliple that value by the total lcd width to get the pixel length of our line
		int progressWidth = percentage * oled.getLCDWidth();
		// the y-position of our line should be at the very bottom of the screen:
		int progressY = oled.getLCDHeight() - 1;
		
		// First, draw a blank line to clear any old lines out:
		oled.line(0, progressY, oled.getLCDWidth(), progressY, BLACK, NORM);
		// Next, draw our line:
		oled.line(0, progressY, progressWidth, progressY);
		oled.display(); // Update the display
	}

	// softwareDebounce keeps our button from "bouncing" around. When a button is
	// pressed, the signal tends to fluctuate rapidly between high and low.
	// This function filters out high-frequency button presses, limiting
	// them to BUTTON_DEBOUNCE_TIME ms.
	bool softwareDebounce()
	{
		// If it's been at least BUTTON_DEBOUNCE_TIME ms since the last press
		if (millis() > (lastButtonPush + BUTTON_DEBOUNCE_TIME))
		{
			lastButtonPush = millis(); // update lastButtonPush
			return true;
		}
		// Otherwise return false.
		return false;
	}

#### Include the SparkFunMicroOLED Library

You're probably used to this now. Follow the same set of steps from the first part of this experiment to include the **SparkFunMicroOLED** library into your application. It should add a `#include "SparkFunMicroOLED/SparkFunMicroOLED.h"` line to the top of your application.

#### Adding Additional Files

Let's throw another wrench into this experiment. Instead of including another library to handle the OpenWeatherMap API -- but to still keep our main main code as clean as possible -- let's add additional files to our application.

To add additional files to an application, **click the "+" icon** in the very upper-right-hand corner of the code window.

-> [![Adding files to an application](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/oled-weather-adding-files.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/oled-weather-adding-files.png) <-

After clicking the plus, two new tabs will appear at the top of the code window -- one .cpp and one .h file. Name these files "OpenWeatherMap".

-> [![New files created](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/oled-weather-extra-files.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/oled-weather-extra-files.png) <-

Then, paste this code into **OpenWeatherMap.h**

	language:c
	/*	OpenWeatherMap.h
		Jim Lindblom <jim@sparkfun.com>
		August 31, 2015
		
		This is a simple library for interacting with the
		OpenWeatherMap API:
		http://openweathermap.org/api
		
		Development environment specifics:
		Particle Build environment (https://www.particle.io/build)
		Particle Photon
		
		Distributed as-is; no warranty is given. 
	*/

	#include "application.h"

	enum t_forecast_type {
	  CURRENT_FORECAST,
	  HOURLY_FORECAST,
	  DAILY_FORECAST
	};

	struct t_forecast_symbol {
	  int number;
	  String name;
	};

	struct t_forecast_precip {
	  String value;
	  String type;
	};

	struct t_forecast_wind {
	  String dirDeg;
	  String dirCode;
	  String dirName;
	  String speedValue;
	  String speedName;
	};

	struct t_forecast_temperature {
	  String value;
	  String minT;
	  String maxT;
	  String nightT;
	  String eveT;
	  String mornT;
	  String unit;
	};

	struct t_forecast_pressure {
	  String unit;
	  String value;
	};

	struct t_forecast_humidity {
	  String value;
	  String unit;
	};

	struct t_forecast_clouds {
	  String value;
	  String all;
	  String unit;
	};

	struct t_current_visibility {
	  String value;
	};

	struct t_forecast {
	  t_forecast_type type;
	  String day;
	  t_forecast_symbol symbol;
	  t_forecast_precip precip;
	  t_forecast_wind wind;
	  t_forecast_temperature temperature;
	  t_forecast_pressure pressure;
	  t_forecast_humidity humidity;
	  t_forecast_clouds clouds;
	  t_current_visibility vis;
	};

	enum t_forecast_units
	{
	  IMPERIAL,
	  METRIC
	};

	// Bitmaps downloaded/converted here: http://luc.devroye.org/fonts-71141.html
	// http://luc.devroye.org/KickstandApps-WeatherIcons-2013.png

	const uint8_t bmp_few_clouds [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0xF0, 0xFF, 0xFF, 0xFC, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0,
	0xF8, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x03, 0x07, 0x1F, 0x3E, 0x7E, 0x7C, 0x3C, 0x18, 0x00, 0x80, 0xC0, 0xE0, 0xF0,
	0xF0, 0xF9, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0x78, 0xF0, 0xF0, 0xE1, 0xE3, 0xC7, 0x87,
	0x03, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0,
	0xE0, 0xF0, 0xF8, 0xF8, 0x7C, 0x3C, 0x3E, 0x1E, 0x1F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x1F, 0x1E, 0x3E, 0x3C, 0x7C, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x00, 0x01, 0x03, 0x07, 0x3F, 0xFF,
	0xFF, 0xFC, 0xC0, 0x00, 0x03, 0x0F, 0x0F, 0x0F, 0x07, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xE0, 0xF0, 0xF8, 0xFC, 0x3E, 0x3E, 0x1E, 0x1F, 0x0F, 0x0F,
	0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0xC0, 0xE0, 0xE3, 0xF7, 0xFF, 0xFF, 0xFF, 0xF8, 0xF0, 0xF0, 0xFE, 0xFF,
	0xFF, 0x9F, 0x01, 0x00, 0xE0, 0xF8, 0xF8, 0xF8, 0xF0, 0xE0, 0xC0, 0xC0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x7E, 0xFF, 0xFF, 0xFF, 0xC3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07,
	0xFF, 0xFF, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x3F, 0x7C, 0x7C, 0x78, 0xF8, 0xF0, 0xF0,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x78, 0x7C, 0x7E,
	0x3F, 0x1F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	const uint8_t bmp_clear_sky [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFE, 0xFE, 0xFC, 0xF0,
	0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0x78, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07,
	0x0E, 0x3E, 0x7C, 0xFC, 0x7C, 0x38, 0x00, 0x80, 0xC0, 0xE0, 0xE0, 0xF0, 0xF1, 0xF1, 0x79, 0x78,
	0x78, 0x78, 0x78, 0x78, 0xF0, 0xF0, 0xF0, 0xE3, 0xC3, 0xC7, 0x87, 0x07, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0xC0, 0xC0, 0xC0, 0xE0,
	0xE0, 0x00, 0x00, 0x00, 0xC0, 0xFC, 0xFE, 0xFF, 0x1F, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x3F, 0xFF, 0xFE, 0xF8, 0x80, 0x00, 0x07,
	0x1F, 0x1F, 0x0F, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x03, 0x03, 0x03, 0x07,
	0x07, 0x00, 0x00, 0x00, 0x03, 0x1F, 0x7F, 0xFF, 0xFC, 0xF0, 0xC0, 0x80, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xC0, 0xF0, 0xFE, 0xFF, 0x7F, 0x1F, 0x01, 0x00, 0xE0,
	0xF8, 0xF8, 0xF0, 0xE0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0,
	0x70, 0x7C, 0x3E, 0x3F, 0x3E, 0x1C, 0x00, 0x00, 0x01, 0x03, 0x07, 0x07, 0x8F, 0x8F, 0x8F, 0x0F,
	0x1E, 0x0E, 0x0F, 0x0F, 0x0F, 0x0F, 0x07, 0xC7, 0xC3, 0xE1, 0xE0, 0xE0, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x7F, 0x7F, 0x3F, 0x0F,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x0F, 0x1E, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	const uint8_t bmp_scattered_clouds [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x80, 0x80, 0xC0, 0xE0, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF8, 0xF8, 0xF8, 0xF8,
	0xF8, 0xF8, 0xF8, 0xF8, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xC0, 0xC0, 0xC0, 0xE0, 0xF8,
	0xFC, 0xFE, 0xFF, 0x7F, 0x1F, 0x0F, 0x0F, 0x07, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x03, 0x03, 0x07, 0x07, 0x0F, 0x1F, 0x3F, 0xFF, 0xFE, 0xFC,
	0xF8, 0xF0, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xC0, 0xF0, 0xF8, 0xFC, 0xFE, 0xFF, 0x3F, 0x1F, 0x1F, 0x0F, 0x0F, 0x07, 0x07, 0x07,
	0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xE0, 0xF0, 0xF0, 0xF1, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFC, 0xF8, 0xF8, 0xF0, 0xF0, 0xE0, 0xE0, 0xC0, 0x80, 0x00, 0x00, 0x00,
	0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x07, 0x0F, 0xFF, 0xFF, 0xFF, 0xFE, 0xFC, 0xE0,
	0x00, 0x07, 0x1F, 0x3F, 0xFF, 0xFF, 0xFE, 0xF8, 0xF0, 0xE0, 0xC0, 0x80, 0x80, 0x80, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0xC0, 0xE0, 0xF0, 0xFF, 0xFF, 0xFF, 0x7F, 0x3F, 0x03,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x07, 0x07, 0x0F, 0x0F, 0x0F, 0x1F, 0x1F, 0x1F,
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F,
	0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x0F, 0x0F, 0x0F, 0x07, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00
	};

	const uint8_t bmp_broken_clouds [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x80, 0xC0, 0xC0, 0xE0, 0xE0, 0xE0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xE0, 0xE0, 0xE0, 0xE0,
	0xC0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF0, 0xF0, 0x78, 0x7C, 0x7E, 0x1F,
	0x0F, 0x07, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03,
	0x03, 0x0F, 0x1F, 0xFF, 0xFC, 0xF8, 0xE0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0,
	0xE0, 0xF0, 0xF0, 0x78, 0x7E, 0x7F, 0x7F, 0x7F, 0x7B, 0x79, 0x70, 0xF0, 0xF0, 0xF0, 0xE0, 0xC0,
	0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x02, 0x07, 0x0F, 0x07, 0x07, 0x07, 0x07, 0x07, 0x07, 0x0F, 0x1F, 0xFE, 0xFC, 0xF8, 0xC0, 0x00,
	0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0x78, 0x78, 0x3C, 0x3E, 0x3F, 0x0F, 0x07, 0x03,
	0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x81, 0x87,
	0xCF, 0xFF, 0xFE, 0xFC, 0xF0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0,
	0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xE0, 0xF0, 0xF8, 0x7F, 0x3F, 0x1F, 0x03, 0x00,
	0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03,
	0x07, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x07, 0x0F, 0xFF, 0xFF, 0xFD, 0xE1, 0x01, 0x01, 0x01,
	0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x03, 0x0F, 0x1F, 0x3F, 0x3E, 0x78, 0x78, 0x70, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0,
	0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0xF0, 0x70, 0x78, 0x7C, 0x3F, 0x1F, 0x0F, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	const uint8_t bmp_shower_rain [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0x78, 0x7C, 0x3C, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
	0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x3C, 0x3C, 0x7C, 0xF8, 0xF0, 0xF0, 0xE0, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xF0, 0xF8, 0xF8, 0x7C, 0x3E,
	0x1E, 0x1E, 0x1F, 0x1F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC1, 0xE7, 0xFF, 0xFF, 0xFF, 0xFC, 0xF0,
	0xE0, 0xE0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xFF, 0xFF, 0xFF, 0xC7, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01,
	0x01, 0x01, 0x03, 0x0F, 0xFF, 0xFF, 0xFE, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x3F, 0x7C, 0x78,
	0xF8, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFC,
	0xFC, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0,
	0xF0, 0xF8, 0x7C, 0x3F, 0x3F, 0x1F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x03, 0x87,
	0x87, 0x07, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xF0, 0xF0, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x1F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xFF,
	0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x1F, 0x1F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	const uint8_t bmp_thunderstorm [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0x78, 0x7C, 0x3C, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
	0x1E, 0x1E, 0x1E, 0x1E, 0x1E, 0x3C, 0x3C, 0x7C, 0xF8, 0xF0, 0xF0, 0xE0, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xF0, 0xF8, 0xF8, 0x7C, 0x3E,
	0x1E, 0x1E, 0x1F, 0x1F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC1, 0xE7, 0xFF, 0xFF, 0xFF, 0xFC, 0xF0,
	0xE0, 0xE0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xFF, 0xFF, 0xFF, 0xC7, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x01,
	0x01, 0x01, 0x03, 0x0F, 0xFF, 0xFF, 0xFE, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x3F, 0x7C, 0x78,
	0xF8, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xFC, 0xFC, 0x0C, 0x00, 0x00,
	0x00, 0x80, 0xE0, 0xF8, 0x7C, 0x1C, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xF0,
	0xF0, 0xF8, 0x7C, 0x3F, 0x3F, 0x1F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x1F, 0x9F, 0xFF, 0xF8, 0xF8, 0xC0, 0xE0,
	0xF2, 0x7B, 0x3F, 0x1F, 0x0E, 0x06, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x78, 0x3F, 0x1F, 0x0F, 0x07, 0x03, 0x01,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	const uint8_t bmp_snow [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0x78, 0x3C, 0x3C, 0x1E, 0x1E, 0x1E, 0x1E, 0x1E,
	0x0E, 0x1E, 0x1E, 0x1E, 0x1E, 0x3E, 0x3C, 0x7C, 0xF8, 0xF0, 0xF0, 0xE0, 0x80, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xC0, 0xF0, 0xF8, 0xF8, 0x7C, 0x3E,
	0x1E, 0x1E, 0x1F, 0x0F, 0x0F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xE1, 0xE7, 0xFF, 0xFF, 0xFF, 0xFC, 0xF0,
	0xE0, 0xE0, 0xC0, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0xFF, 0xFF, 0xFF, 0xC7, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x01,
	0x01, 0x01, 0x03, 0x0F, 0xFF, 0xFF, 0xFE, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x0F, 0x1F, 0x3F, 0x7C, 0x78,
	0xF8, 0xF0, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x37, 0xBE, 0xFC,
	0x7E, 0x7F, 0xD9, 0x98, 0x18, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0xF0, 0xF0,
	0xF0, 0xF8, 0x7C, 0x3F, 0x3F, 0x1F, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0xC0, 0x00, 0x80, 0xC0, 0x80, 0x00, 0x00, 0x00, 0x03, 0x01,
	0x00, 0x00, 0x01, 0x01, 0x00, 0x80, 0x80, 0x98, 0xF8, 0xE0, 0xE0, 0xF8, 0x98, 0x80, 0x80, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x06, 0x66, 0x76, 0x3F, 0x1F, 0x7E, 0xFF, 0x0D, 0x0C, 0x08, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x1D, 0x0F, 0x07, 0x07, 0x0F, 0x1D, 0x01, 0x01, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};

	const uint8_t bmp_haze [] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xF0, 0xFE,
	0xFF, 0xFC, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x1F,
	0x7F, 0xFE, 0xFE, 0xFC, 0x78, 0x30, 0x00, 0x00, 0x00, 0x80, 0x80, 0xC0, 0xC0, 0xC3, 0xC3, 0xC3,
	0xC3, 0xC3, 0xC3, 0xC0, 0xC0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x10, 0x38, 0x78, 0xFC, 0xFE, 0xFF,
	0x3F, 0x0F, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
	0x00, 0x01, 0xC0, 0xE0, 0xF8, 0xFC, 0xFE, 0x7F, 0x3F, 0x1F, 0x0F, 0x0F, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x0F, 0x1F, 0x1F, 0x7F, 0xFE, 0xFC, 0xF8, 0xF0, 0xE0, 0x81, 0x00,
	0x00, 0x00, 0x00, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x07, 0x07, 0x0F, 0x0F, 0x1F, 0x1F, 0x1F, 0x07, 0x00, 0x00,
	0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8,
	0x00, 0x00, 0x00, 0x1F, 0x1F, 0x1F, 0x0F, 0x0F, 0x07, 0x07, 0x07, 0x03, 0x01, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x30, 0x31, 0x31, 0x31, 0x31, 0x31, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31, 0x31, 0x31, 0x31, 0x31, 0x30,
	0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x07, 0x07, 0x07,
	0x07, 0x07, 0x67, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7,
	0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0xE7, 0x47, 0x07,
	0x07, 0x07, 0x07, 0x07, 0x07, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	};


	class OpenWeather
	{
	public:
	  OpenWeather();
	  OpenWeather(String apiKey);

	  int current(float latitude, float longitude);
	  int current(String cityName);
	  int current(uint32_t cityID);

	  int hourly(float lat, float lon, unsigned int tripleHours);
	  int hourly(String cityName, unsigned int tripleHours);
	  int hourly(uint32_t cityID, unsigned int tripleHours);

	  int daily(float lat, float lon, unsigned int days);

	  void setUnits(t_forecast_units units);

	  float temperature();
	  float maxTemperature();
	  float minTemperature();
	  unsigned int humidity();
	  float pressure();

	  String getDate();
	  String getTime();

	  float windSpeed();
	  String windDirection();
	  String windName();

	  float precipitationValue();
	  String precipitationType();

	  int conditionID();
	  String conditionName();

	  t_forecast forecast;

	private:
	  String _apiKey;
	  t_forecast_units _units;

	  int currentForecast(String location);
	  int hourlyForecast(String location, unsigned int count);
	  int dailyForecast(String location, unsigned int count);

	  String parseXML(String * search, String tag, String attribute);
	  int tagIndex(String * xml, String tag, bool start);

	  void printDebug();
	};

And paste this code into **OpenWeatherMap.cpp**

	language:c
	/*	OpenWeatherMap.cpp
		Jim Lindblom <jim@sparkfun.com>
		August 31, 2015
		
		This is a simple library for interacting with the
		OpenWeatherMap API:
		http://openweathermap.org/api
		
		Development environment specifics:
		Particle Build environment (https://www.particle.io/build)
		Particle Photon
		
		Distributed as-is; no warranty is given. 
	*/

	#include "OpenWeatherMap.h"

	String weatherServer = "api.openweathermap.org";

	OpenWeather::OpenWeather()
	{
	  _apiKey = NULL;
	  _units = IMPERIAL;
	}

	OpenWeather::OpenWeather(String apiKey)
	{
	  _apiKey = apiKey;
	}

	void OpenWeather::setUnits(t_forecast_units units)
	{
	  _units = units;
	}

	int OpenWeather::current(float lat, float lon)
	{
	  return currentForecast("lat=" + String(lat) + "&lon=" + String(lon));
	}

	int OpenWeather::current(String cityName)
	{
	  return currentForecast("q=" + cityName);
	}

	int OpenWeather::current(uint32_t cityID)
	{
	  return currentForecast("id=" + String(cityID));
	}

	int OpenWeather::hourly(float lat, float lon, unsigned int tripleHours)
	{
	  return hourlyForecast("lat=" + String(lat) + "&lon=" + String(lon), tripleHours);
	}

	int OpenWeather::hourly(String cityName, unsigned int tripleHours)
	{
	  return hourlyForecast("q=" + cityName, tripleHours);
	}

	int OpenWeather::hourly(uint32_t cityID, unsigned int tripleHours)
	{
	  return hourlyForecast("id=" + String(cityID), tripleHours);
	}

	int OpenWeather::daily(float lat, float lon, unsigned int days)
	{
	  if (days > 10)
		return -1;
	  return dailyForecast("lat=" + String(lat) + "&lon=" + String(lon), days);
	}

	float OpenWeather::temperature()
	{
	  return forecast.temperature.value.toFloat();
	}

	float OpenWeather::maxTemperature()
	{
	  return forecast.temperature.maxT.toFloat();
	}

	float OpenWeather::minTemperature()
	{
	  return forecast.temperature.minT.toFloat();
	}

	unsigned int OpenWeather::humidity()
	{
	  return forecast.humidity.value.toInt();
	}

	float OpenWeather::pressure()
	{
	  return forecast.pressure.value.toFloat();
	}

	String OpenWeather::getDate()
	{
	  int timeStart = forecast.day.indexOf('T');

	  return forecast.day.substring(0, timeStart);
	}

	String OpenWeather::getTime()
	{
	  if (forecast.type == DAILY_FORECAST)
		return NULL;

	  int timeStart = forecast.day.indexOf('T');
	  return forecast.day.substring(timeStart + 1);
	}

	float OpenWeather::windSpeed()
	{
	  return forecast.wind.speedValue.toFloat();
	}

	String OpenWeather::windDirection()
	{
	  return forecast.wind.dirCode;
	}

	String OpenWeather::windName()
	{
	  return forecast.wind.speedName;
	}

	String OpenWeather::precipitationType()
	{
	  if ((forecast.precip.type == "no") || (forecast.precip.type == NULL))
	  {
		return NULL;
	  }
	  return forecast.precip.type;
	}

	float OpenWeather::precipitationValue()
	{
	  return forecast.precip.value.toFloat();
	}

	int OpenWeather::conditionID()
	{
	  return forecast.symbol.number;
	}

	String OpenWeather::conditionName()
	{
	  return forecast.symbol.name;
	}

	int OpenWeather::currentForecast(String location)
	{
	  /////////////////////////////////////////
	  // Connect to client and send HTTP GET //
	  /////////////////////////////////////////
	  TCPClient client;
	  if (client.connect(weatherServer, 80))
	  {
		  client.print("GET /data/2.5/weather?");
		  client.print(location);
		  client.print("&mode=xml");
		  if (_apiKey != NULL)
			client.print("&APPID=" + _apiKey);
		  if (_units == IMPERIAL)
			client.print("&units=imperial"); // Change imperial to metric for celsius (delete this line for kelvin)
		  else if (_units == METRIC)
			client.print("&units=metric");
		  client.println(" HTTP/1.0");
		  client.println("Host: " + weatherServer);
		  client.println("Content-length: 0");
		  client.println();
	  }
	  else
	  {
		  return 0;
	  }

	  ///////////////////////////////////////
	  // Wait for Response, Store in Array //
	  ///////////////////////////////////////
	  String rsp;
	  int timeout = 1000;
	  while ((!client.available()) && (timeout-- > 0))
		delay(1);
	  if (timeout <= 0)
		return 0;
	  while (client.available())
	  {
		char c = client.read();
		rsp += c;
	  }

	  ////////////////
	  // Disconnect //
	  ////////////////
	  client.stop();
	  Serial.println("Response: ");
	  Serial.println(rsp);

	  //////////////////////////////
	  // Sort Data into Variables //
	  //////////////////////////////
	  forecast.temperature.value = "" + parseXML(&rsp, "temperature", "value");
	  forecast.temperature.minT = "" + parseXML(&rsp, "temperature", "min");
	  forecast.temperature.maxT = "" + parseXML(&rsp, "temperature", "max");
	  forecast.temperature.unit = "" + parseXML(&rsp, "temperature", "unit");
	  forecast.humidity.value = "" + parseXML(&rsp, "humidity", "value");
	  forecast.humidity.unit = "" + parseXML(&rsp, "humidity", "unit");
	  forecast.pressure.value = "" + parseXML(&rsp, "pressure", "value");
	  forecast.pressure.unit = "" + parseXML(&rsp, "pressure", "unit");
	  forecast.wind.speedValue = "" + parseXML(&rsp, "speed", "value");
	  forecast.wind.speedName = "" + parseXML(&rsp, "speed", "name");
	  forecast.wind.dirDeg = "" + parseXML(&rsp, "direction", "value");
	  forecast.wind.dirCode = "" + parseXML(&rsp, "direction", "code");
	  forecast.wind.dirName = "" + parseXML(&rsp, "direction", "name");
	  forecast.clouds.all = "" + parseXML(&rsp, "clouds", "value");
	  forecast.clouds.value = "" + parseXML(&rsp, "clouds", "name");
	  forecast.vis.value = "" + parseXML(&rsp, "visibility", "value");
	  forecast.precip.value = "" + parseXML(&rsp, "precipitation", "value");
	  forecast.precip.type = "" + parseXML(&rsp, "precipitation", "mode");
	  forecast.symbol.number = parseXML(&rsp, "weather", "number").toInt();
	  forecast.symbol.name = "" + parseXML(&rsp, "weather", "value");
	  forecast.day = "" + parseXML(&rsp, "lastupdate", "value");
	  forecast.type = CURRENT_FORECAST;

	  //printDebug();

	  return 1;
	}

	int OpenWeather::hourlyForecast(String location, unsigned int count)
	{
	  /////////////////////////////////////////
	  // Connect to client and send HTTP GET //
	  /////////////////////////////////////////
	  TCPClient client;
	  if (client.connect(weatherServer, 80))
	  {
		  client.print("GET /data/2.5/forecast?");
		  client.print(location);
		  client.print("&cnt=" + String(count));
		  client.print("&mode=xml");
		  if (_apiKey != NULL)
			client.print("&APPID=" + _apiKey);
		  if (_units == IMPERIAL)
			client.print("&units=imperial"); // Change imperial to metric for celsius (delete this line for kelvin)
		  else if (_units == METRIC)
			client.print("&units=metric");
		  client.println(" HTTP/1.0");
		  client.println("Host: " + weatherServer);
		  client.println("Content-length: 0");
		  client.println();
	  }
	  else
	  {
		  return 0;
	  }

	  ///////////////////////////////////////
	  // Wait for Response, Store in Array //
	  ///////////////////////////////////////
	  String rsp;
	  unsigned int forecastCount = 0;
	  int timeout = 1000;
	  while ((!client.available()) && (timeout-- > 0))
		delay(1);
	  if (timeout <= 0)
		return 0;
	  while (client.available())
	  {
		char c = client.read();
		rsp += c;
		if (rsp.indexOf("</time>") > 0)
		{
		  forecastCount++;
		  if (forecastCount < count)
		  {
			rsp = NULL; // Clear out response
		  }
		  else
		  {
			break;
		  }
		}
	  }

	  ////////////////
	  // Disconnect //
	  ////////////////
	  client.stop();
	  Serial.println("Response: ");
	  Serial.println(rsp);

	  forecast.day = "" + parseXML(&rsp, "time", "from");
	  forecast.symbol.number = parseXML(&rsp, "symbol", "number").toInt();
	  forecast.symbol.name = "" + parseXML(&rsp, "symbol", "name");
	  forecast.precip.value = "" + parseXML(&rsp, "precipitation", "value");
	  forecast.precip.type = "" + parseXML(&rsp, "precipitation", "type");
	  forecast.wind.dirDeg = "" + parseXML(&rsp, "windDirection", "deg");
	  forecast.wind.dirCode = "" + parseXML(&rsp, "windDirection", "code");
	  forecast.wind.dirName = "" + parseXML(&rsp, "windDirection", "name");
	  forecast.wind.speedValue = "" + parseXML(&rsp, "windSpeed", "mps");
	  forecast.wind.speedName = "" + parseXML(&rsp, "windSpeed", "name");
	  forecast.temperature.value = "" + parseXML(&rsp, "temperature", "value");
	  forecast.temperature.minT = "" + parseXML(&rsp, "temperature", "min");
	  forecast.temperature.maxT = "" + parseXML(&rsp, "temperature", "max");
	  forecast.temperature.nightT = "" + parseXML(&rsp, "temperature", "night");
	  forecast.temperature.eveT = "" + parseXML(&rsp, "temperature", "eve");
	  forecast.temperature.mornT = "" + parseXML(&rsp, "temperature", "morn");
	  forecast.pressure.unit = "" + parseXML(&rsp, "pressure", "unit");
	  forecast.pressure.value = "" + parseXML(&rsp, "pressure", "value");
	  forecast.humidity.value = "" + parseXML(&rsp, "humidity", "value");
	  forecast.humidity.unit = "" + parseXML(&rsp, "humidity", "unit");
	  forecast.clouds.value = "" + parseXML(&rsp, "clouds", "value");
	  forecast.clouds.all = "" + parseXML(&rsp, "clouds", "all");
	  forecast.clouds.unit = "" + parseXML(&rsp, "clouds", "unit");
	  forecast.type = HOURLY_FORECAST;

	  //printDebug();

	  return 1;
	}

	int OpenWeather::dailyForecast(String location, unsigned int count)
	{
	  /////////////////////////////////////////
	  // Connect to client and send HTTP GET //
	  /////////////////////////////////////////
	  TCPClient client;
	  if (client.connect(weatherServer, 80))
	  {
		  client.print("GET /data/2.5/forecast/daily?");
		  client.print(location);
		  client.print("&cnt=" + String(count));
		  client.print("&mode=xml");
		  if (_apiKey != NULL)
			client.print("&APPID=" + _apiKey);
		  if (_units == IMPERIAL)
			client.print("&units=imperial"); // Change imperial to metric for celsius (delete this line for kelvin)
		  else if (_units == METRIC)
			client.print("&units=metric");
		  client.println(" HTTP/1.0");
		  client.println("Host: " + weatherServer);
		  client.println("Content-length: 0");
		  client.println();
	  }
	  else
	  {
		  return 0;
	  }

	  ///////////////////////////////////////
	  // Wait for Response, Store in Array //
	  ///////////////////////////////////////
	  String rsp;
	  unsigned int forecastCount = 0;
	  int timeout = 1000;
	  while ((!client.available()) && (timeout-- > 0))
		delay(1);
	  if (timeout <= 0)
		return 0;
	  while (client.available())
	  {
		char c = client.read();
		rsp += c;
		if (rsp.indexOf("</time>") > 0)
		{
		  forecastCount++;
		  if (forecastCount < count)
		  {
			rsp = NULL; // Clear out response
		  }
		  else
		  {
			break;
		  }
		}
	  }

	  ////////////////
	  // Disconnect //
	  ////////////////
	  client.stop();
	  Serial.println("Response: ");
	  Serial.println(rsp);

	  forecast.day = "" + parseXML(&rsp, "time", "day");
	  forecast.symbol.number = parseXML(&rsp, "symbol", "number").toInt();
	  forecast.symbol.name = "" + parseXML(&rsp, "symbol", "name");
	  forecast.precip.value = "" + parseXML(&rsp, "precipitation", "value");
	  forecast.precip.type = "" + parseXML(&rsp, "precipitation", "type");
	  forecast.wind.dirDeg = "" + parseXML(&rsp, "windDirection", "deg");
	  forecast.wind.dirCode = "" + parseXML(&rsp, "windDirection", "code");
	  forecast.wind.dirName = "" + parseXML(&rsp, "windDirection", "name");
	  forecast.wind.speedValue = "" + parseXML(&rsp, "windSpeed", "mps");
	  forecast.wind.speedName = "" + parseXML(&rsp, "windSpeed", "name");
	  forecast.temperature.value = "" + parseXML(&rsp, "temperature", "day");
	  forecast.temperature.minT = "" + parseXML(&rsp, "temperature", "min");
	  forecast.temperature.maxT = "" + parseXML(&rsp, "temperature", "max");
	  forecast.temperature.nightT = "" + parseXML(&rsp, "temperature", "night");
	  forecast.temperature.eveT = "" + parseXML(&rsp, "temperature", "eve");
	  forecast.temperature.mornT = "" + parseXML(&rsp, "temperature", "morn");
	  forecast.pressure.unit = "" + parseXML(&rsp, "pressure", "unit");
	  forecast.pressure.value = "" + parseXML(&rsp, "pressure", "value");
	  forecast.humidity.value = "" + parseXML(&rsp, "humidity", "value");
	  forecast.humidity.unit = "" + parseXML(&rsp, "humidity", "unit");
	  forecast.clouds.value = "" + parseXML(&rsp, "clouds", "value");
	  forecast.clouds.all = "" + parseXML(&rsp, "clouds", "all");
	  forecast.clouds.unit = "" + parseXML(&rsp, "clouds", "unit");
	  forecast.type = DAILY_FORECAST;

	  //printDebug();

	  return 1;
	}

	String OpenWeather::parseXML(String * search, String tag, String attribute)
	{
		int tagStart = tagIndex(search, tag, 1);
		int tagEnd = tagIndex(search, tag, 0);
		if (tagStart >= 0)
		{
		  int attributeStart = search->indexOf(attribute, tagStart);
		  if ((attributeStart >= 0) && (attributeStart < tagEnd)) // Make sure we don't get value of another key
		  {
			attributeStart = search->indexOf("\"", attributeStart);
			if (attributeStart >= 0)
			{
				int attributeEnd = search->indexOf("\"", attributeStart + 1);
				if (attributeEnd >= 0)
				{
					return search->substring(attributeStart + 1, attributeEnd);
				}
			}
		  }
		}

		return NULL;
	}

	int OpenWeather::tagIndex(String * xml, String tag, bool start)
	{
	  String fullTag = "<";
	  if (start)
	  {
		fullTag += tag;
		fullTag += ' '; // Look for a space after the tag name
	  }
	  else
	  {
		fullTag += '/';
		fullTag += tag;
		fullTag += '>';
	  }

	  return xml->indexOf(fullTag);
	}

	void OpenWeather::printDebug()
	{
	  Serial.println("=====================================================");
	  Serial.print("day: "); Serial.println(forecast.day);
	  Serial.print("symbol.number: "); Serial.println(forecast.symbol.number);
	  Serial.print("symbol.name: "); Serial.println(forecast.symbol.name);
	  Serial.print("precip.value: "); Serial.println(forecast.precip.value);
	  Serial.print("precip.type: "); Serial.println(forecast.precip.type);
	  Serial.print("wind.dirDeg: "); Serial.println(forecast.wind.dirDeg);
	  Serial.print("wind.dirCode: "); Serial.println(forecast.wind.dirCode);
	  Serial.print("wind.dirName: "); Serial.println(forecast.wind.dirName);
	  Serial.print("wind.speedValue: "); Serial.println(forecast.wind.speedValue);
	  Serial.print("wind.speedName: "); Serial.println(forecast.wind.speedName);
	  Serial.print("temperature.value: "); Serial.println(forecast.temperature.value);
	  Serial.print("temperature.minT: "); Serial.println(forecast.temperature.minT);
	  Serial.print("temperature.maxT: "); Serial.println(forecast.temperature.maxT);
	  Serial.print("temperature.unit: "); Serial.println(forecast.temperature.unit);
	  Serial.print("pressure.unit: "); Serial.println(forecast.pressure.unit);
	  Serial.print("pressure.value: "); Serial.println(forecast.pressure.value);
	  Serial.print("humidity.value: "); Serial.println(forecast.humidity.value);
	  Serial.print("humidity.unit: "); Serial.println(forecast.humidity.unit);
	  Serial.print("clouds.value: "); Serial.println(forecast.clouds.value);
	  Serial.print("clouds.all: "); Serial.println(forecast.clouds.all);
	  Serial.print("vis.value: "); Serial.println(forecast.vis.value);
	  Serial.println("=====================================================");
	}

If you click back to the main application code, and scroll to the top, you'll notice it automatically added `#include "OpenWeatherMap.h"` for you. Convenient.

Adding additional source files is a great way to functionally separate parts of your code, without having to go through the library-including process.

#### Add your OpenWeather API Key and Latitude/Longitude

Last thing before weather forecasting! There are three variables in the main source file to change. First, copy your **OpenWeatherMap API key**, and paste it into the `OPENWEATHER_API_KEY` String near the top of the code -- between the quotes.

	language:c
	// Example: Defining an OpenWeatherMap API key as a String
	const String OPENWEATHER_API_KEY = "42tHisISaNeXampLEApIKey123456890";

Then, get your **latitude and longitude**, and paste it into the `LATIITUDE` and `LONGITUDE` variables right below the API key.

	language:c
	// Example: Defining the lat and long as float variables:
	const float LATITUDE = 40.090554;
	const float LONGITUDE = -105.184861;

Edits done! Flash away!

### Code to Note

We've written a simplified class to interact with the OpenWeatherMap API and server called `OpenWeather`. That's what's included in the two OpenWeatherMap extra files. To use the class, begin by creating an `OpenWeather` object, like this:

    language:c
    // Create an OpenWeather object, giving it our API key as the parameter:
    OpenWeather weather(OPENWEATHER_API_KEY);

The OpenWeatherMap API allows you to get the **current** weather status, or a **future forecast** -- hours or days from now. To gather the current forecast for a set location, there are a few options:

    language:c
    // To get the weather for a latitude/longitude location:
    weather.current(float latitude, float longitude); // Get the weather at a specific latitude and longitude.
    
    // To get a weather update for a city name, call current(String cityName).
    // cityName can be the lone name of a city, e.g. "Denver"
    // or it can be a "city,countrycode", like "Denver,US"
    weather.current(String cityName); // Get the weather for a specific city name or city/country code.
    
    // Or, to be really specific about it, pass the current function your city ID.
    // WHich can be grabbed from here: http://bulk.openweathermap.org/sample/
    weather.current(uint32_t cityID);

Once you've made a call to the `weather.current()` function -- and it succeeds, returning a 1 -- you can use a number of get functions to read the temperature, humidity, precipitation forecast and more. Some examples include:

    language:c
    float temperature = weather.temperature(); // Get the average temperature
    unsigned int humidity = weather.humidity(); // Get the relative humidity %
    float pressure = weather.pressure(); // Get the pressure, in hPa

    float windSpd = weather.windSpeed(); //get the wind speed in mps
    String windDir = weather.windDirection(); // Get wind compass direction, like "S", "NW", etc.
    String windDescription = weather.windName(); // Get the wind description, like "Breezy"

    String precipType = precipitationType(); // Get the type of precipitation "NONE", "Rain", "snow"
    float precipAmount = weather.precipitationValue(); // Get amount of precip in mm

There are a few more, like `weather.conditionName()`, which are all demonstrated in the experiment code. Check through the comments to see how they're used.

### What You Should See

When the code begins to run, your Photon RedBoard will connect to the OpenWeatherMap server, and gather all of the data it can regarding the **current weather** forecast. There's a lot of data to display! It's not all going to fit on the Micro OLED. Initially the "big 3" pieces of weather data will be displayed: temperature, relative humidity, and pressure.

-> `Photo: big three on display. Video?` <-

The app is configured to cycle the display every 10 seconds -- the progress of that 10-second cycle is displayed by the line at the bottom of the display. If you've read everything you can on the current screen, press the **green button** to cycle to the next one.

To refresh the current weather forecast, click the **yellow button**.

Our application can also **predict** the weather! To see into the future, click the **blue button**. The first blue button click will get the forecast for 3 hours from now. The same display cycle will occur, this time with data for 3 hours from now. Successive blue button clicks will forecast the weather 6, then 9 hours, then 1, 2, and 3 days.

-> `Forecast video?` <-

<div class="alert alert-danger"><b>Don't spam the buttons!</b> Each time you click the blue or yellow buttons, the Photon will request a weather forecast from the OpenWeatherMap server. There are limits to how often your free API key can make requests of the OpenWeatherMap server. Don't exceed it!</div>

### Troubleshooting

To help diagnose any issues, `Serial.println()`'s are scattered through the program. If you're having any trouble, try opening a serial port to your Photon to see what it says.

If you've made it past the first part of the experiment, you should have already verified the OLED and button circuits are working. Is weather data not being displayed? Maybe your Photon RedBoard isn't able to communicate with the OpenWeatherMap server. Try checking the status of the OpenWeatherMap server [is it down for just you?](http://downforeveryoneorjustme.com/openweathermap.org).