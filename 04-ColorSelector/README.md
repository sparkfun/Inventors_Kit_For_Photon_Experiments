###Introduction

In this experiment you'll learn about analog input and output, the difference between analog and digital, and how to incorporate analog inputs and outputs into your project. We will also touch on some more advanced concepts, like using internal pull-up resistors and integrating with Twitter via IFTTT (If This Than That).

####Parts Needed

You will need the following parts:

* **1x** Breadboard
* **1x** Photon RedBoard
* **1x** Potentiometer
* **3x** Simon Buttons (Red, Green, and Blue)
* **11x** Jumper Wires

<div class="alert alert-info"><b> Using a Photon by Particle instead or you don't have the kit?</b> No worries! You can still have fun and follow along with this experiment. We suggest using the parts below:</div>

<!-- products_by_id(9806, 10302, 12795) -->

####Suggested Reading

There are a variety of core concepts in electronics that we will be touching on in this circuit, but not discussing in depth. However, we do have some great tutorials that go into more detail about what's going on behind the scenes.

* [Analog vs. Digital](https://learn.sparkfun.com/tutorials/analog-vs-digital) - understanding the difference between analog and digital devices is going to be very helpful for this section.
* [Pulse-Width Modulation](https://learn.sparkfun.com/tutorials/pulse-width-modulation) - pulse-width modulation (or PWM) is the way digital microcontrollers simulate analog output.
* [Analog to Digital Conversion](https://learn.sparkfun.com/tutorials/analog-to-digital-conversion) - knowing how your microcontroller translates between digital and analog signals will help you understand many of the basics covered here.

###Hardware Hookup

Connect the components on the breadboard and wire them to the Photon RedBoard as shown below. The red button should go to pin D2, the green button to D3, and the blue button to pin D4. The potentiometer should go to pin A0. Don't forget to run power (3.3V) and ground (GND) from your Photon to your breadboard.

-> [![alt text](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/PhotonSIK_Circuit4.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/PhotonSIK_Circuit4.png) <-

->_Having a hard time seeing the circuit? Click on the Fritzing diagram to see a bigger image._<-

###Digital vs. Analog (A Very Short Summary)

If you haven't guessed by now, the letter in front of each pin number refers to either D (Digital) or A (Analog) - but how do we know which one to use for each component? Basically, we want to think about the number of states or possible values a part can have: digital parts have only 2 states - ON or OFF (TRUE or FALSE, 1 or 0, etc.), while analog parts can have a *range* of states or values (think about the brightness of a light, or the volume control on your computer).

So, in general, a smooth range of values means analog, 2 (or more) discrete values means digital. That's why our buttons, which can only be pressed or not pressed at any given time, are connected to digital pins. Our potentiometer (which is really just a fancy word for dial or knob) can be at a wide range of values, so it gets connected to an analog pin.

###Photon RedBoard Code Part 1

If you're use to Arduino, this part's a little different. Instead of downloading and installing an IDE, you'll need to open up an Internet browser and open this link (preferably on another tab or window): 

**[www.particle.io/build](https://www.particle.io/build)**

It is a bit of an adjustment at first, but it ends up being fantastic since you can program your Photon RedBoard from anywhere in the world as long as you have an Internet connection. 

Go ahead and open the first code example from this repo in the Particle Build IDE.
    

###Code to Note

`RGB.control(true);`

You may have noticed by now that there is no LED on your breadboard. Instead, we're going to take control of the RGB LED on the Photon RedBoard that's usually reserved for showing the status of the board. We do this by using the built-in RGB library and setting control to us, the user. 
<br>

`pinMode(redButton, INPUT_PULLUP);`

Push buttons like the ones we're using operate by closing or opening a circuit when you push down the button. The Photon can detect this change and report it to us so we know when someone pushes our buttons. Often, buttons are hooked up to the breadboard with a 'pull-up' resistor (usually 10KΩ) which in essence *pulls* the voltage reading from the button to HIGH. This means that when we push the button the value goes LOW, which seems to make sense to us logically. Luckily for us, the Photon has *internal* pull-up resistors that we can turn on through the code - by changing the pinMode type from the usual `INPUT` to `INPUT_PULLUP`.
<br>


###What You Should See

After the you click 'flash' and the upload process is complete, you should be able to control the color and brightness of the Photon RedBoard's onboard LED. The buttons will turn the LED red, green, or blue, and turning the potentiometer will affect the brightness of the LED.

Switching colors:

-> [![alt text](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/output_gsNYua.gif)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/output_gsNYua.gif) <-

Fading:

-> [![alt text](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/output_Ss2d2W.gif)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/output_Ss2d2W.gif) <-


Pretty neat, huh? Now, let's take this circuit and make it a part of the Internet of Things!

###Troubleshooting

* If your buttons aren't working, make sure they are pushed down firmly into the breadboard and that you declared them as `INPUT_PULLUP` in your code.
* If the LED is still breathing Cyan, double check that you put in the `RGB.control(true);` line in your `setup()` function.

##Part 2: Turn on an LED with IFTTT (If This Then That)

[**IFTTT**](https://ifttt.com) is a website that uses conditional statements and does very useful things with well known applications, such as Gmail, Craigslist, Twitter, or Facebook. Such as, **Tweet a color to your LED** -- which is what we're about to do!




###Photon Code Part 2

Since we'll be needing [IFTTT to communicate with our Photon RedBoard]((http://docs.particle.io/ifttt/), we'll need to modify our code. Particle.io has created many useful IoT [functions](http://docs.particle.io/firmware/) for the Photon RedBoard, and we'll be using one of them -- more specifically [Particle.function()](http://docs.particle.io/firmware/#particle-function). In our case, we're going to find a way to 'tweet' an RGB color value, and have the onboard LED of the Photon RedBoard turn that color.

Go ahead and open the second code examples in the Particle Build IDE.

###Code to Note

`Particle.function("rgbColor", rgbColor);`
    
Particle.function is a function specifically made for communication with IFTTT. It works as Particle.function("cloudNickname", firmwareFunctionName). The cloud name can be maximum 12 characters long. There's a great write up for this provided by Particle.io, click [HERE](http://docs.particle.io/firmware/#particle-function). 

Manipulating groups of words and symbols, or 'strings' is a key component in many programs - so much so that Particle has a String 'class' - which allows us to use several different pre-built methods for dealing with strings. In fact, use *four* of them just in this exercise: `length()`, `indexOf()`, `substring()`, and `toInt()`. More info on these and other useful methods for strings can be found in the Particle docs [here](https://docs.particle.io/reference/firmware/photon/#string-class).

###Setup IFTTT

Now that we've prepped our code to talk with the IFTTT service, we have to actually sign up for it and create our Internet of Things 'recipe'. If you're completing the exercises in order, you will have signed up for IFTTT in the last exercise, if not, go ahead and sign up now.

* Sign up, or log into **[IFTTT](https://ifttt.com)**, and activate your account from your email address. 
* Create your first recipe:
  * Click on the blue lettered: "This"
  * Type Twitter into the search bar and click on it
  * Connect your Twitter account to IFTTT, hit continue
  * On the "Choose a Trigger" page, select "New tweet by @yourtwitter with hashtag"
  * For the hashtag, type in #rgb
  
You should see something like:

-> [![alt text](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Screen_Shot_2015-08-25_at_12.08.10_PM.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Screen_Shot_2015-08-25_at_12.08.10_PM.png) <-

  * Click on "Create trigger" 
  * Click on "That"
  * Search for "Particle", click on it
  * Choose "Call a function", and select the function we put in our photon code: "rgbColor on (your Photon RedBoard's name)". 
  * In the "with input(Function Input)" field, choose TextNoHashtag
  
  You should see:

-> [![alt text](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Screen_Shot_2015-08-25_at_12.07.49_PM.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Screen_Shot_2015-08-25_at_12.07.49_PM.png) <-
  
  * Finally, click on "Create Action" and finally "Create Recipe"

Great! You've just created an IFTTT recipe that calls our `rgbColor()` function whenever we send a tweet with a specific hashtag. 


###What You Should See

Send a tweet with the hashtag #rgb followed by numbers for red, green, and blue values. You might have to allow the gears of the Internet to churn for a minute or two, but you should eventually see your LED turn on. If you don't feel like waiting, go ahead and click on the refresh-looking icon in the IFTTT dashboard for your recipe, named "Check Recipe now".

Your tweet should look something like:

-> [![alt text](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Screen_Shot_2015-08-31_at_2.32.40_PM.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Screen_Shot_2015-08-31_at_2.32.40_PM.png) <-

###Troubleshooting

* If the LED is still breathing Cyan, double check that you put in the `RGB.control(true);` line in your `setup()` function.
* If the function name doesn't show up when trying to complete the 'call a function' step (6 of 7), make sure your board is plugged in, and that you've saved your code in the cloud with the `Particle.function("turnOnLED", LEDstate);` line in your code.
* If the function *still* doesn't show up, you may have to go into IFTTT, delete your Particle channel (and all your recipes), then reconnect the channel and rebuild your recipe from scratch.

###Going Further

We've just scraped the beginning of analog input (your potentiometer) and analog output (the brightness and colors of the RGB LED). Try changing some of the code to get the red, green, and blue LED's to mix. Try altering your IFTTT recipe to change the LED color based on a text from your phone, or the temperature in Tahiti. Let us know what you came up with! 

