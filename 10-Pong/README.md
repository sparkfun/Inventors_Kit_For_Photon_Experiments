###Introduction
In this experiment, we'll use the OLED Breakout, potentiometer, and a photo resistor to make a game of Pong, where the paddles are controlled by the analog values of the two sensors.

-> [![alt text](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/Photon_SIK_Tutorial_-03a.jpg)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Photon_SIK_Tutorial_-03a.jpg) <-

###Parts Needed

You will need the following parts (besides your Photon RedBoard and Breadboard, of course):

* **1x** OLED Breakout Board
* **1x** Potentiometer
* **1x** Photoresistor
* **1x** 330 Ohm Resistor
* **15x** Jumper Wires

<div class="alert alert-info"><b> Using a Photon by Particle instead or you don't have the kit?</b> No worries! You can still have fun and follow along with this experiment. We suggest using the parts below:</div>

<!-- products_by_id(13003, 9806, 9088, 8377) -->

###Suggested Reading

* [OLED Breakout Hookup Guide](https://learn.sparkfun.com/tutorials/micro-oled-breakout-hookup-guide) - if you're stuck hooking up the OLED breakout, check this lovely guide for more info
* [What is an Arduino?](tutorials/50) -- We'll use an Arduino to send commands and display data to the OLED.
* [Serial Peripheral Interface (SPI)](tutorials/16) -- SPI is the preferred method of communication with the display.
* [I<sup>2</sup>C](tutorials/82) -- Alternatively, I<sup>2</sup>C can be used to control the display. It uses less wires, but is quite a bit slower.
* [How to Use a Breadboard](tutorials/47) -- The breadboard ties the Arduino to the OLED breakout.

###Hardware Hookup

We'll be connecting three devices to our Photon RedBoard - the OLED breakout, a trim potentiometer, and a photocell. These last two are analog sensors that are going to act as the paddle controllers for each player, while the OLED screen displays the ball, the paddles, and the score. 

-> [![alt text](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/Circuit9_bb_3.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Circuit9_bb_4.png) <-

->_Having a hard time seeing the circuit? Click on the Fritzing diagram to see a bigger image._<-

The photocell is hooked up to pin A0 and GND though a **330** (NOT 10K) Ohm resistor from one pin, while the other pin is connected to +3.3V. The potentiometer's middle pin goes to pin A1, while the side pins are connected to GND and 3.3V.

Here's a handy table for the OLED Breakout connections:

<table class="table table-striped table-condensed">
<th class="col-md-2">Photon RedBoard Pin</th>
<th class="col-md-3">OLED Breakout Pin</th>
<th class="col-md-7"></th>
<tr>
<td>A2</td>
<td>CS</td>
<td></td>
</tr>
<tr>
<td>A3</td>
<td>SCK</td>
<td></td>
</tr>
<tr>
<td>A5</td>
<td>SDI</td>
<td></td>
</tr>
<tr>
<td>D5</td>
<td>D/C</td>
<td></td>
</tr>
<tr>
<td>D6</td>
<td>RST</td>
<td></td>
</tr>
<tr>
<td>3V3</td>
<td>3.3V</td>
<td></td>
</tr>
<tr>
<td>GND</td>
<td>GND</td>
<td></td>
</tr>
</table>


The table and Fritzing image are much more helpful, but if you wanted to see inside the box, here's a shot:

INSERT breadboard pic here

###Photon Code

Pong is a great programming exercise for getting used to a new language or environment. There are tons of great pong tutorials out there, so we're not going to focus too much on the general code beyond the parts that interact with the hardware.

The first thing we need to do after creating a new app is to add the SparkFun OLED library to our sketch. This let's us draw and write all manner of things to the OLED screen quite easily. First, click on the bookmark icon in the sidebar (it's the fourth one up from the bottom). Under 'Community Libraries', there should be a search box - type in OLED, and you should see the 'SparkFunMicroOLED' Library pop up, like so:

-> [![alt text](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Screen_Shot_2015-06-29_at_2.04.53_PM.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Screen_Shot_2015-06-29_at_2.04.53_PM.png) <-

Click on the library name, and then on the big blue button that says "Include in App". That should cause a line like `#include "SparkFunMicroOLED/SparkFunMicroOLED.h"` to appear near the top of your sketch. Voila!

Now that you've imported the library, you can copy and paste the code from the first example in this repository.


###Code to Note

Whew! That's a lot of code to digest all at once. Let's look at a few of the key sections that make this work.

Defining our pin assignments and declaring our OLED object happens right at the top:

    language:c
    #define PIN_RESET D6  // Connect RST to pin 6
    #define PIN_DC    D5  // Connect DC to pin 5 (required for SPI)
    #define PIN_CS    A2 // Connect CS to pin A2 (required for SPI)
    MicroOLED oled(MODE_SPI, PIN_RESET, PIN_DC, PIN_CS);
    
    //#define SINGLE_PLAYER
    
    const int player1Pin = A1; //connected to potentiometer
    #ifndef SINGLE_PLAYER
    const int player2Pin = A0; //connected to photocell
    #endif
    
This is also where we decide a few important things: which communication mode we're using - SPI in our case means we put `MODE_SPI` as that first variable in the MicroOLED object. Below that, we're also deciding if we want a 1 player or 2 player game. To make the game one player, simply uncomment the `//#define SINGLE_PLAYER` line.

The next section is crucial - it's where we 'calibrate' our sensors, by telling the program what we expect the minimum and maximum values for each sensor to be. Changes are you'll have to run the program first and look at the serial monitor while testing your sensors to get a good 'sense' of the value ranges. 

    language:c
    /*** Sensor Calibration ****/

    int sensor1Calibration = LCDHEIGHT; //photocell w/330
    int sensor2Calibration = LCDHEIGHT; //potentiometer
    
    //potentiometer
    int sensor1Min = 0;
    int sensor1Max = 4096;
    //photocell
    int sensor2Min = 100;
    int sensor2Max = 1000;

For example, if you're in a very brightly lit room, your photocell will get a wider range of values than if you're in a dimly lit place -- but we still want the paddle to move the entire range of the screen, which is why we save the `LCDHEIGHT` value to a separate variable to use later on.

After we set a number of global variables that control things like how fast the ball moves and what number each game goes until (the winning number), we're on to our `setup()` and `loop()` functions.

    language:c
    void setup()
    {
      // in our setup, we call a few custom functions to get everything ready 
      initializeGraphics();
      initializeInput();
      displayGameStart();
      Serial.begin(9600);
    }
    
    void loop()
    {
      updateGame(); //custom function to advance the game logic
      renderGame(); //custom function to dislay the current game state on the OLED screen
      // print out sensor values via serial so we can calibrate
      Serial.print(analogRead(A0));
      Serial.print(" <-A0 | A1-> ");
      Serial.println(analogRead(A1));
    
      //winning conditions
      if (player1Score >= scoreToWin)
      {
        gameOver(true);
      }
      else if (player2Score >= scoreToWin)
      {
        gameOver(false);
      }
    }

You'll notice that we make use of a lot of custom functions in order to keep our main setup and loop short and readable. In the `setup()`, we call a few functions that help start up the OLED screen, initialize our input pins, display the start screen, and open up serial communication. The `loop()` is simply updating the game state, displaying that state, checking to see if anyone won, and ending the game if so. Dig into all of these functions to get a better sense of how the game works!

###What You Should See

After successfully flashing the code onto your Photon RedBoard, you should see the words 'Get Ready!' on the OLED screen, followed soon after by the familiar Pong game screen. If you've calibrated your sensors well, you should be able to move each paddle up and down to defend your side. After a player wins, the game should start all over again.

If you turn the potentiometer or place your hand over the photocell, the paddles should move up and down, allowing you to play a game of Pong:

-> [![alt text](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/Photon_SIK_Tutorial_-02.jpg)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Photon_SIK_Tutorial_-02.jpg) <-



###Troubleshooting

The main challenge (besides being good at Pong) is making sure your sensor inputs are calibrated to the point where each paddle covers the entire height of the OLED screen. Checking the serial monitor while testing your sensors is the best way to get an idea of what the range for each sensor is.

##Part 2: Scoreboard!

What does every serious Pong competition need? A scoreboard, that's what! In this half of the exercise, we're going to use Particle.variable() with the Particle Cloud API to create a web page that stores our Pong scores locally (i.e., without a database) in HTML 5 localData, displays them in a table, and updates when a new score in ready.


-> [![alt text](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/Photon_SIK_Tutorial_-03b.jpg)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Photon_SIK_Tutorial_-03b.jpg) <-

###Photon Code

Now we'll be using the part 2 code from this repo - in reality, there are just a few changes. We'll go over them in the next section, but the code here is the complete sketch on the Photon side.


###Code to Note: Photon Part 2

Some important bits have changed, so pay attention.

    
    //score keeper
    char postScore[64];
    
    //game ID
    int gameID = 0;
    
We add to variable at the top of our code to contain `postScore[]` a character array that we will register as our Particle.variable(), and `gameID`, which we will use as a unique identifier for each game played.

In `setup()`, we register our `postScore[]` variable so we can access from the cloud:

    Particle.variable("data", &postScore, STRING);
    
That first argument ("data" in our case) is what we will use in the Particle api to retrieve our info - but it could be called anything.

Now for the fun part - formatting our data to a string to be accessed by the Particle cloud API.

    // OK - here's the new bit
    void gameOver(bool didWin)
    {
      //at the end of a game, increment the gameID by 1        
      gameID += 1;
      //now, send out the player 1 score, player 2 score, and the game ID
      //we do this by using 'sprintf', which takes a bunch of random data types (in our case, integers),
      //and puts them all into a nicely formatted string - which is exactly what our Particle.variable is supposed to be
      sprintf(postScore, "{\"player1Score\":%d, \"player2Score\":%d, \"gameID\":%d}", player1Score, player2Score, gameID);
      
Read the comments for a better idea of what's going on, or if you're feeling very adventurous, check out the [c++ reference page](http://www.cplusplus.com/reference/cstdio/sprintf/) for `sprintf`.

###HTML Code

This is where the real action is happening for this exercise - on the web. We'll be using a combination of good ol' HTML, the HTML 5 localStorage ability, and some basic javaScript to create a page that reads the values of our Particle.variable, stores them (without using a database), displays them in a table, and checks every 15 seconds for a new score to come in.

Create a new html file in your favorite text editor, save it as 'PongScoreboard.html' and paste in the code from this repo.
    

###Code to Note: HTML

Ok, so it looks like a mess of gibberish right now - that's fine. The **most important thing** is that you find the URL variable and change it to match your credentials. It looks like this (should be around line 39):

`var url = "https://api.particle.io/v1/devices/{your device name}/{your Particle.variable name}?access_token={your access_token}";`

The parts in curly brackets {} are the parts you need to replace. If even one thing is off, none of this will work. Computers are pretty finicky that way, trust me. A great way to check is to paste your URL in a browser (make sure your Photon code is uploaded and your Photon RedBoard is plugged in first) - you should see that URL return a JSON object (it's a nice way of organizing data for the web). It might look something like this (I took bits out for privacy):

    {
    "cmd": "VarReturn",
    "name": "data",
    "result": "{\"player1Score\":10, \"player2Score\":2, \"gameID\":1}",
    "coreInfo": {
    "last_app": "",
    "last_heard": "2015-08-26T21:35:28.021Z",
    "connected": true
    }

If you see something like, you're all set - if not, check your credentials and try again.

###What You Should See

Plug in your Photon RedBoard, and make sure it's running Pong and posting a JSON object to the URL you specified. Now, if you open the HTML file in your browser of choice (the URL might be something like 'file://localhost/Users/User1/Desktop/Experiment10.html' on a mac), you should see something like this:

-> [![alt text](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Screen_Shot_2015-08-27_at_3.19.29_PM.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Screen_Shot_2015-08-27_at_3.19.29_PM.png) <-

If the page is blank, give it a minute or two - the updates are sometimes a little out of sync with what the Pong game is currently doing (and remember, it only posts new data after a game has been won or lost).

The page will refresh every 15 seconds, check for a new score, add it if there is one, and show it to you. You can push the 'clear scores' button to empty the scores table, though to reset the gameID you'll have to restart your Photon.

Sweet! Now, as long as your Photon is online you can point your friends to a live scoreboard of your match scores.

###Troubleshooting

Granted, this is a rather complicated experiment (I'm not even sure that anyone had done this with a Photon yet when we wrote this). Being comfortable with HTML and javaScript is obviously a big help here. Here are a few links that might help you out (they helped me out, anyway) while looking through the code and comments:

* [Restoring an array from HTML5 localStorage](http://stackoverflow.com/questions/17263008/restoring-application-data-array-after-refresh-html-5-localstorage)
* [Iterating over a JSON object](http://stackoverflow.com/questions/1078118/how-do-i-iterate-over-a-json-structure)
* [Parsing JSON for an HTML table](http://stackoverflow.com/questions/17066636/parsing-json-objects-for-html-table)
* [Using InnerHTML and javaScript](http://www.quackit.com/javascript/tutorial/innerhtml_in_javascript.cfm)

###Going Further

There is SO much potential to go further with this project. Display the data with the last game played at the top of the table. Add some CSS and make it look pretty. Keep track of player 1 wins vs. player 2 wins over time. Calculate the average margin of victory. Visualize the scores in a graph over time. So. Many. Possibilities. Please share with us if you make some improvements!
