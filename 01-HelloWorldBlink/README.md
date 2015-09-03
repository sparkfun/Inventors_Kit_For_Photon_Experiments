###Introduction

LEDs (light-emitting diodes) are small, powerful lights that are used in many different applications. To start off the SIK for Photon, we will work on blinking an LED. Chances could be you have blinked on an LED before, or perhaps this is your first time -- but have you turned on an LED with the Internets? Blinking an LED is the most basic "Hello World" test for hardware, but now we're going to be saying "Hello Internet." This experiment will also walk you through uploading your first sketch with the Particle IDE. 

####Parts Needed

You will need the following parts:

* **1x** Breadboard
* **1x** Photon
* **1x** LED (Choose any color in the bag full of LEDs)
* **1x** 330Ω Resistor
* **2x** Jumper Wires

####Suggested Reading

Before continuing on with this experiment, we recommend you be familiar with the concepts in the following tutorial:

* [LEDs (Light-emitting Diodes)](https://learn.sparkfun.com/tutorials/light-emitting-diodes-leds) -- LEDs are found everywhere. Learn more about LEDs and why they are used in some many products all over the world.
* [Resistors](https://learn.sparkfun.com/tutorials/resistors?_ga=1.67007470.1330965882.1426512336) -- Why use resistors? Learn more about resistors and why they are important in circuits like this one. 
* [How to Use a Breadboard](https://learn.sparkfun.com/tutorials/how-to-use-a-breadboard) -- Never used a breadboard before? Make sure to read this tutorial before continuing.
* [What is a Circuit?](tutorials/26) --  This tutorial will explain what a circuit is, as well as discuss voltage in further detail.
* [Voltage, Current, Resistance, and Ohm's Law](tutorials/27) -- Learn the basics of electricity. 
* [Digital vs Analog](https://learn.sparkfun.com/tutorials/analog-vs-digital) -- In this experiment, you will be hooking up an LED to a digital pin on the Photon RedBoard. This means your LED can be either ON or OFF. Learn more about the differences of using the digital pins vs the analog pins. 

###Hardware Hookup

Ready to party? Components like resistors need to have their legs bent into 90° angles in order to correctly fit the breadboard sockets. You can also cut the legs shorter to make them easier to work with on the breadboard. 

[![Bent resistor](https://cdn.sparkfun.com/assets/learn_tutorials/2/7/5/resistor_bent_legs_drawing.png)](https://cdn.sparkfun.com/assets/learn_tutorials/2/7/5/resistor_bent_legs_drawing.png)

Pay close attention to the LED. The negative side of the LED is the short leg, marked with a flat edge.

[![LED drawing](https://cdn.sparkfun.com/assets/learn_tutorials/2/7/5/LED_drawing_01.png)](https://cdn.sparkfun.com/assets/learn_tutorials/2/7/5/LED_drawing_01.png)

Each experiment will have a [Fritzing](http://fritzing.org/home/) hook-up diagram. Connect the components to the breadboard and Photon RedBoard by following the Fritzing diagram below:

Pay special attention to the component’s markings indicating how to place it on the breadboard. Polarized components can only be connected to a circuit in one direction. Orientation matters for the following component: LED

[![Photon Particle Blink LED](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/sik_photon_exp_01_blue_led.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/sik_photon_exp_01_blue_led.png) 


Having a hard time seeing the circuit? Click on the Fritzing diagram to see a bigger image.

All jumper wires work the same. They are used to connect two points together. All the experiments will show the wires with different colored insulations for clarity, but using different combinations of colors is completely acceptable. 

Be sure to the polarity on the LED is correct. The longer lead should be connected to D0.

###Time for some code!

Now to the exciting part! You'll need to go to the **[Particle Build IDE](https://build.particle.io)** (preferably on another tab or window) to get started. If you are not familiar with the Particle Build IDE, please refer to the [Particle Build IDE documentation](https://docs.particle.io/guide/getting-started/build/photon/#web-ide) or the [Particle Build IDE]() overview in the beginning of this guide. 

Using an online IDE might be little different at first for some users, but it ends up being fantastic. You can program your Photon from anywhere in the world as long as you have an Internet connection. 

Double check your Photon RedBoard's RGB LED is breathing cyan. If it is, then your Photon RedBoard is connected to the Internet and ready to go. If your Photon RedBoard is not breathing cyan, please refer to the [Connecting your Photon to WiFi documentation](https://docs.particle.io/guide/getting-started/connect/photon/). If you go to the navigation bar, click on **Devices** to see if your Photon RedBoard is connected.

[![Screen shot of devices for Photon RedBoard.](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/ScreenShotDeviceFound.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/ScreenShotDeviceFound.png)

There will be a breathing cyan dot next to your Photon RedBoard in the **Devices** section.

For each experiment, we recommend creating a new app. Go to the **Code** section in the navigation bar on the left. The hit the **CREATE NEW APP** button. After typing in the name, you can hit enter.

[![Name app](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/ScreenShotBlink_1.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/ScreenShotBlink_1.png)

When you create a new app, particle build creates an .ino file and renames the tab in the Particle Build editor.

Let's add the code! Copy and paste the code below into the in Particle Build editor. 

    language:c
    /*	SparkFun Inventor's Kit for Photon
        Experiment 1 - Part 1: Hello World Blink an LED
        This sketch was written by SparkFun Electronics
    	August 31, 2015
    	https://github.com/sparkfun
    	
    	This is a simple example sketch that turns on an LED 
    	for one second, off for one second, and repeats forever.
    
    	Development environment specifics:
    	Particle Build environment (https://www.particle.io/build)
    	Particle Photon RedBoard
    	Released under the MIT License(http://opensource.org/licenses/MIT)
    */
    
    int led = D0; // We name pin D0 as led
    
    // This routine runs only once upon reset
    void setup()
    {
      pinMode(led, OUTPUT); // Initialize D0 pin as output
    }
    
    // This routine loops forever
    void loop()
    {
      digitalWrite(led, HIGH);  // Turn ON the LED
      delay(1000);              // Wait for 1000mS = 1 second
      digitalWrite(led, LOW);   // Turn OFF the LED
      delay(1000);              // Wait for 1 second
    }

Once you've pasted the code into the window, let's get in the habit of checking our code. Hit **Verify** (circle icon with a check mark in the top left of the Particle Build's IDE navigation bar) to check if there is any errors in the code. Next hit **Save** (folder icon) to save our code, so we can come back to the code later if we want to.

Time for a drum roll please! Hit **Flash** (lightning bolt icon) to load the code onto the Photon RedBoard. 

###Code to Note

First we name pin D0 as led.

`int led = D0;` 

The Photon RedBoard has digital input/output pins. These pins configured as either inputs or outputs. We set this up with a built-in function called `pinMode()`. The `pinMode()` function takes two values, which you type in the parenthesis after the function name. The first value is a pin number, the second value is the word INPUT or OUTPUT.

Here we'll set up pin 0 (the one connected to a LED) to be an output. We're doing this because we need to send voltage "out" of the Photon RedBoard to the LED.

`pinMode(led, OUTPUT);`
    
When you're using a pin as an OUTPUT, you can command it to be HIGH (output 3.3 volts), or LOW (output 0 volts).

`digitalWrite(led, HIGH);`
    

There are different ways to make comments in your code. Comments are a great way to quickly describe what is happening in your code. 

`// This is a comment - anything on a line after "//" is ignored
// by the computer.`

`/* This is also a comment - this one can be multi-line, but it
must start and end with these characters */`



###What You Should See

Once you click **Flash**, you should be seeing your Photon RedBoard flash magenta, followed by green, and then back to breathing Cyan. Now your LED should be blinking. You've just wirelessly programmed your Photon RedBoard to blink an LED, from the cloud! 

[![Photon RedBoard LED hookup](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/Inventors_Kit_For_Photon-Experiment_Guide-11.jpg)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Inventors_Kit_For_Photon-Experiment_Guide-11.jpg)

Great job on successfully flashed your first sketch. If you want to learn how blink an LED with the Tinker mobile app, you can head to part 2 of this experiment! 


###Troubleshooting
* **Code refuses to flash** -- Try putting your Photon RedBoard into [Safe Mode](blob:https%3A//vine.co/960aa66c-df55-4420-ab42-e3203853c2c0). Then, hit **Flash**. Sometimes to get your Photon RedBoard breathing cyan again, unplugging from USB and replugging back can get your board connecting to WiFi again. Please keep in mind if you just starting working with the Photon RedBoard it might take a couple minutes for the firmware to upload when first connected to the Internet. 

* **LED isn't blinking** -- Try checking your connections again. Make sure the positive side of the LED is connected to D0. It is really easy to put the jumper wire in the wrong hole on the Photon RedBoard. 


* **Photon RedBoard RGB isn't doing anything** -- Don't worry! We have an awesome tech support staff to help you out if there is something wrong. Please [contact](https://www.sparkfun.com/static/technical_assistance) our tech support team. 

##Part 2: Blink an LED with Tinker

[Tinker](https://docs.particle.io/guide/getting-started/tinker/photon/) is a mobile app for iPhone or Android smartphones that makes it easy to control your digital and analog pins on your Photon or Photon RedBoard.

First you will need to download the app on your smartphone. 

* [iPhone users](https://itunes.apple.com/us/app/particle-build-photon-electron/id991459054?ls=1&mt=8)

* [Android users](https://play.google.com/store/apps/details?id=io.particle.android.app)

* **Windows users** -- :( Unfortunately, this extra feature is only for iOS and Android devices. Don’t worry, this is the only bonus material that works with the Tinker app.

* **Non-smartphone users** -- Most of us would love to go back the happy memories of not owning a smartphone and not getting 50+ alerts each day. We applaud you! We also might secretly envy you. Unfortunately, this bonus material isn't going to be your cup of tea. Don’t worry, this is the only bonus material that works with the Tinker app. 

When the app is done downloading on your phone, you will need to sign in with your login information. After logging in, you will see a list with all your Photons. 

[![Photon Device ID](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/ScreenshotDeviceID.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/ScreenshotDeviceID.png)

Clicking on the **more** icon (three dots) next to the online dot, gives you more options to choose from. For example, you can rename your board here. 

<div class="alert alert-warning"><span class="glyphicon glyphicon-warning-sign"></span><b> When clicking on the more icon, do not choose Re-Flash Tinker. </b> At the moment, it will re-flash your board with firmware that isn't the latest and greatest with the Photon RedBoard! </div>

We suggest naming your Photon RedBoard with unique names. When you start having a lot of Photons and demos going at once, it is hard to remember which is which.

When you click over the name of the Photon RedBoard you want to work with, you will see a new screen with a list of the different pins you can control. 

[![List of Photon RedBoard pins](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/ScreenshotTinkerPins.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/ScreenshotTinkerPins.png)

First click the circle that is label with **D0**. Anytime you want to work with a pin, you will need to select it first. You can select multiple pins at a time.

[![alt text](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/ScreenshotTinkerPinSelect.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/ScreenshotTinkerPinSelect.png)

You notice that you have a couple different options. For this example, we are going to control the LED ON and OFF, by clicking on **digitalWrite**. 

[![alt text](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/ScreenshotTinkerDigitalWrite.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/ScreenshotTinkerDigitalWrite.png)

###New Photon Code (There is none!)

What is great about this mobile app, you do not have to do any coding! Once you are signed into Tinker, you can start playing with the pins. 

###What You Should See

To turn the LED ON/HIGH, click the circle labeled **D0** again. It should show the LED turn on.

[![alt text](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/ScreenshotTinkerHigh.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/ScreenshotTinkerHigh.png)

To turn the LED OFF/LOW, click the circle labeled **D0** again. It should show the LED turn off.

[![alt text](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/ScreenshotTinkerLow.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/ScreenshotTinkerLow.png) 

Super fun and easy way to control an LED. To reset all the pins and start fresh, click on the **more** icon (three dots) on the top right. You will see **Reset all pin functions** option. 

Dig into the app a little bit more! What happens when you click **D0** and choose **analogWrite**? What happens when you select **D7** instead? HINT: Take at look at your board!

###Troubleshooting

* **The app froze** -- If your app ever stops on you, close out the app and reopen. Report bugs to the wonderful Particle team on their [forums](https://community.particle.io/). 

* **I have a Windows phone** -- :( Unfortunately, this extra feature is only for iOS and Android devices. Don't worry, this is the only bonus material that works with the Tinker app. 

* **There is a yellow circle and it says it is non-tinker** -- All Photon RedBoards were shipped out with the latest firmware that works with Tinker. If you get the yellow circle, it means the firmware on your RedBoard was re-flashed with old firmware. You will need to flash with the latest firmware. Please follow the directions [here](https://github.com/spark/firmware/releases) for now. We are working with Particle to make sure the Tinker app is pushing the latest firmware when re-flashing. 
