/*	SparkFun Inventor's Kit for Photon
        Experiment 11 - Part 2: OLED Weather App
        This sketch was written by SparkFun Electronics
	Jim Lindblom <jim@sparkfun.com>
	August 31, 2015
	https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments
	
	This application uses an API provided by OpenWeatherMap.org to
	find the current or future weather forecast for a specified location.

	Development environment specifics:
	Particle Build environment (https://www.particle.io/build)
	Particle Photon RedBoard
	Released under the MIT License(http://opensource.org/licenses/MIT)
*/

// !!! REQUIRED LIBRARIES !!!
// Before trying to flash this app, include the SparkFunMicroOled library!
// https://github.com/sparkfun/SparkFun_Micro_OLED_Particle_Library

// OpenWeatherMap.h defines the OpenWeather class.
// This should be in the same directory as WeatherApp.ino
#include "OpenWeatherMap.h" 

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
const String OPENWEATHER_API_KEY = "2f1da5ecf8d4f03e492042783405eb17"; 
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