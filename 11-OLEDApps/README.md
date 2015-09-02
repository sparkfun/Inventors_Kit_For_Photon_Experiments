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

Find this code in [Code/01_DigitalWatch/DigitalWatch.ino](https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments/blob/master/11_OLED_Apps/Code/01_DigitalWatch/DigitalWatch.ino).

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

Find this code in [Code/02_WeatherForecast/WeatherApp.ino](https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments/blob/master/11_OLED_Apps/Code/01_DigitalWatch/DigitalWatch.ino).

#### Include the SparkFunMicroOLED Library

You're probably used to this now. Follow the same set of steps from the first part of this experiment to include the **SparkFunMicroOLED** library into your application. It should add a `#include "SparkFunMicroOLED/SparkFunMicroOLED.h"` line to the top of your application.

#### Adding Additional Files

Let's throw another wrench into this experiment. Instead of including another library to handle the OpenWeatherMap API -- but to still keep our main main code as clean as possible -- let's add additional files to our application.

To add additional files to an application, **click the "+" icon** in the very upper-right-hand corner of the code window.

-> [![Adding files to an application](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/oled-weather-adding-files.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/oled-weather-adding-files.png) <-

After clicking the plus, two new tabs will appear at the top of the code window -- one .cpp and one .h file. Name these files "OpenWeatherMap".

-> [![New files created](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/oled-weather-extra-files.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/oled-weather-extra-files.png) <-

Then, paste this code into **OpenWeatherMap.h**

Find this code in [Code/02_WeatherForecast/OpenWeather.h](https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments/blob/master/11_OLED_Apps/Code/02_WeatherForecast/OpenWeather.h)

And paste this code into **OpenWeatherMap.cpp**

Find this code in [Code/02_WeatherForecast/OpenWeather.cpp](https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments/blob/master/11_OLED_Apps/Code/02_WeatherForecast/OpenWeather.cpp)

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