# Introduction

The [SparkFun Inventor's Kit for Photon](https://www.sparkfun.com/products/13320), or SIK for Photon for short, is the latest and greatest in Internet of Things kits. Whether you're a software developer just getting in to hardware, an electronics engineer learning software, or somewhere in between, this kit will help you get your projects hooked up to the Internet in no time. 

For an overview of the Photon RedBoard and a preview of the kinds of experiments you'll get to build with this kit, check out the video below. 

-> <!-- youtube(https://www.youtube.com/watch?v=RP2ow-N9VD0) --> <-

<div class="alert alert-warning"><b><span class="glyphicon glyphicon-time"></span> Set Aside Some Time</b> - Each experiment in this kit has two parts, with the second half usually containing an Internet-connected component. Please allow yourself ample time to complete each experiment. You may not get through all the experiments in one sitting.</div>

### Included Materials 

Here is a complete list of all the parts included in the SIK for Photon. 

-> [![Photon Inventor's Kit Parts](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Photon_Kit.jpg)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Photon_Kit.jpg) <-


The SparkFun Inventor's Kit for Photon Includes the following: 

<ul>
<li><a href="https://www.sparkfun.com/products/13321">SparkFun Photon RedBoard</a></li>
<li><a href="https://www.sparkfun.com/products/11235">Photon RedBoard and Breadboard Holder</a></li>
<li><a href="https://www.sparkfun.com/products/12002">White Solderless Breadboard</a></li>
<li><a href="https://www.sparkfun.com/products/12891">Pocket Screwdriver Set</a></li>
<li><a href="https://www.sparkfun.com/products/9065">Small Servo</a></li>
<li><a href="https://www.sparkfun.com/products/10218">9V Alkaline Battery</a></li>
<li><a href="https://www.sparkfun.com/products/9518">9V to Barrel Jack Adapter</a></li>
<li><a href="https://www.sparkfun.com/products/10215">USB microB Cable - 6 Foot</a></li>
<li><a href="https://www.sparkfun.com/products/12795">Jumper Wires</a></li>
<li><a href="https://www.sparkfun.com/products/9750">JST Right Angle Connector - Through-Hole 3-Pin</a></li>
<li><a href="https://www.sparkfun.com/products/13322">Soil Moisture Sensor</a></li>
<li><a href="https://www.sparkfun.com/products/13003">SparkFun Micro OLED Breakout</a> (with Headers)</li>
<li><a href="https://www.sparkfun.com/products/12756">SparkFun Triple Axis Accelerometer Breakout - MMA8452Q</a> (with Headers)</li>
<li><a href="https://www.sparkfun.com/products/13285">PIR Motion Sensor (JST)</a></li>
<li><a href="https://www.sparkfun.com/products/10167">RHT03 Humidity and Temperature Sensor</a></li>
<li><a href="https://www.sparkfun.com/products/13247">Magnetic Door Switch Set</a></li>
<li><a href="https://www.sparkfun.com/products/9088">Photocell</a></li>
<li><a href="https://www.sparkfun.com/products/12062">Red, Blue, Yellow, and Green LEDs</a></li>
<li><a href="https://www.sparkfun.com/products/10302">Red, Blue, Yellow, and Green Tactile Buttons</a></li>
<li><a href="https://www.sparkfun.com/products/9806">10K Trimpot</a></li>
<li><a href="https://www.sparkfun.com/products/7950">Piezo Speaker</a></li>
<li><a href="https://www.sparkfun.com/products/11507">330 Ohm Resistors</a></li>
</ul>

If, at any time, you are unsure which part a particular experiment is asking for, reference this section. 

###Suggested Reading

The following links are here to help guide you in your journey through the SIK for the Photon. Referencing these documents throughout this guide will help you get the most out of this kit. 

* [The Photon RedBoard Hookup Guide](https://learn.sparkfun.com/tutorials/photon-redboard-hookup-guide) - This guide goes over the features of the Photon RedBoard in great detail, from the functions of each pin to a compare and contrast between the Photon RedBoard, the Photon, and the classic Arduino Uno. 
* [Photon Development Guide](https://learn.sparkfun.com/tutorials/photon-development-guide) - Learn how to develop with your Photon or Photon RedBoard using the three different methods described in this tutorial. 
* [Getting Started with Particle](http://docs.particle.io/start/) - The Particle website has tons of great documentation to get you started in the world of IoT development. 

# Using the Kit
Before embarking upon the experiments, there are a few items to cover first. If you have completed one of our other Inventor's Kits before, you should already be familiar with most of the concepts in this section. If this is your first Inventor's Kit, please read carefully to ensure the best possible SIK experience. 

####Photon RedBoard 

The [SparkFun Photon RedBoard](https://www.sparkfun.com/products/13321) is an over-the-air-programmable WiFi development board that is compatible with the [Particle](https://particle.io) cloud, and it is the heart of the SIK for Photon. To learn more about the Photon RedBoard and all it's functionality, visit the [Photon Redboard Hookup Guide](https://learn.sparkfun.com/tutorials/photon-redboard-hookup-guide). 

-> <!-- tutorial_big(421) --> <-

####Base Plate 

To make circuit development easier, we have included a baseplate onto which you can attach your breadboard and your Photon RedBoard. 

To attach the breadboard, peel off the adhesive backing, and place the breadboard on the baseplate, making sure that the SparkFun logo and text on your breadboard all face the same direction. 

To attach the Photon RedBoard, use the included screws and screwdriver to attach the board tot he baseplate. Again, be sure that the text on the pins matches the directions of the breadboard text and the SparkFun logo. The USB connector should be pointing up when looking directly at the baseplate. 

-> [![base plate](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/Inventors_Kit_For_Photon-Experiment_Guide-14.jpg)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Inventors_Kit_For_Photon-Experiment_Guide-14.jpg) <-


####Breadboard

Solderless breadboards are the go to prototyping tool for those getting started with electronics. If you have never used a breadboard before, we recommend reading through our [How to Use a Breadboard tutorial](https://learn.sparkfun.com/tutorials/how-to-use-a-breadboard) before starting with the experiments. 

-> <!-- tutorial_big(47) --> <-

####Jumper Wires

This kit includes twenty 6" long jumper wires terminated as male to male. Multiple jumpers can be connected next to one another on a 0.1" header or breadboard.

-> [![jumpers](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/jumpOff.jpg)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/jumpOff.jpg) <-

Each group of jumpers are connected to each other and can either be pulled apart in any quantity or kept whole based on you needs.

-> [![peeling](https://cdn.sparkfun.com/r/400-400/assets/learn_tutorials/3/3/2/Jumper_wire_peel.jpg)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/Jumper_wire_peel.jpg) <-

####Screwdriver 

Last, we've included a pocket screwdriver set to aid you in any mechanical portions of this guide. Unscrew the cap on the tail end of the screwdriver to reveal the various tips that can be inserted into the head of the screwdriver. 

-> [![alt text](https://cdn.sparkfun.com/r/400-400/assets/learn_tutorials/3/3/2/screwdriver1.jpg)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/screwdriver1.jpg) <-

You will need to swap out tips for various tasks throughout this guide. 

-> [![alt text](https://cdn.sparkfun.com/r/400-400/assets/learn_tutorials/3/3/2/screwdriver2.jpg)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/screwdriver2.jpg) <-

# Using the Particle IDE

If you've worked with Arduino or with our SparkFun Inventor's Kit for Arduino, then you are familiar with the Arduino IDE, short for Integrated Development Environment. Particle has created their own cloud-based IDE, and they have adopted the Arduino language and syntax allowing you to move from an Arduino to the Photon and Photon RedBoard with ease. 

Particle has written a great getting started guide for using their Web IDE, called Particle Build. You can read through their documentation by following the link below. 

-> <!-- button(Getting Started with Particle Build, https://docs.particle.io/guide/getting-started/build/photon/) --> <-

We have also written a Photon Development guide to help aid you in your experience. There are numerous ways to develop with the Photon and Photon RedBoard, and this guide covers the three most common methods: Particle Build, Particle Dev, and ARM GCC. 

-> <!-- tutorial_big(400) --> <-

For the purposes of this guide, we recommend sticking to the online Particle Build IDE. However, once you feel comfortable using the Photon RedBoard, you are free to explore the other methods for development. 

# Getting Started with the Photon RedBoard

The Photon RedBoard can be powered over either USB (using the included [Micro-B Cable](https://www.sparkfun.com/products/13244)) or with a 4.5-15V barrel jack power supply (like either our [5V](https://www.sparkfun.com/products/12889) and [9V](https://www.sparkfun.com/products/298) wall warts). To begin using your Photon RedBoard, plug it in!

-> [![USB plugged into Photon RedBoard](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/4/2/1/action-usb-2.jpg)](https://cdn.sparkfun.com/assets/learn_tutorials/4/2/1/action-usb-2.jpg) <-

The red "POWER" LED should illuminate to indicate the Photon RedBoard is on. Once that's verified, it's time to set up WiFi.

### Configuring WiFi, Connecting to Your Particle Account

To use the Particle cloud -- and their online IDE -- you'll need a Particle account. Head over to [build.particle.io](https://build.particle.io/signup) to sign up, if you haven't already.

-> <!-- button(Create a Particle Account!, https://build.particle.io/signup) --> <-

When you power on a Photon RedBoard for the first time, it should boot up into **listening mode** -- indicated by a blinking, blue LED. It'll remain in listening mode until configured with your WiFi network and password.

There are a handful of ways to configure a Photon's WiFi credentials, including with the **Particle smartphone app** (iOS8+ or Android), or through a **serial terminal**. Unless you're very comfortable with [serial terminals](tutorials/112) -- or just don't have a smartphone nearby -- we recommend using the app.

<div class="alert alert-warning"><b>Windows users:</b> To use the Photon in serial port mode, Windows users will need to install a driver. <a href="https://docs.particle.io/guide/getting-started/connect/core/#installing-the-particle-driver">Follow Particle's "Installing the Particle Driver" guide</a> for help installing the driver.</div>

Both setup methods are described below, click one of the buttons to expand your section of interest:

<div class="panel-group" id="accordion" role="tablist" aria-multiselectable="false">
  <div class="panel panel-default">
    <div class="panel-heading" role="tab" id="headingOne">
      <h3 style="text-align:center;" class="panel-title">
        <a class="btn btn-default" role="button" data-toggle="collapse" data-parent="#accordion" href="#collapseOne" aria-expanded="false" aria-controls="collapseOne">
          Configuring WiFi with the Particle App
        </a>
      </h3>
    </div>
    <div id="collapseOne" class="panel-collapse collapse" role="tabpanel" aria-labelledby="headingOne">
      <div class="panel-body">
        <h4>Step 1: Download the Particle App</h4>
        <p>The Partice app is available for both iOS (8.0 and up) and Android phones: <a href="https://itunes.apple.com/us/app/particle-build-photon-electron/id991459054?ls=1&mt=8">iPhone</a> | <a href="https://play.google.com/store/apps/details?id=io.particle.android.app">Android</a>.</p>
        <h4>Step 2: Follow the in-app directions to connect your Photon</h4>
        <p>1. After opening the app, press "Get Started", to arrive at the device list screen. Scroll to the bottom and select <b>Setup a Photon...</b></p>
        <p style="text-align:center;"><a href="https://cdn.sparkfun.com/assets/learn_tutorials/4/2/1/app-device-list-2.PNG"><img src="https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/4/2/1/app-device-list-2.PNG"></img></a></p>
        <p style="text-align:center;"><em>Select "Setup a Photon..." to begin setting up your Photon RedBoard.</em></p>
        <p>2. Verify that your Photon RedBoard's RGB LED is blinking blue, then select "READY" on the next screen.</p>
        <p>3. Navigate to your phone's WiFi settings and find a network named something like <b>Photon-9XYZ</b> (with a random set of four characters as the suffix). Select it.</p>
        <p style="text-align:center;"><a href="https://cdn.sparkfun.com/assets/learn_tutorials/4/2/1/app-wifi-list.PNG"><img src="https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/4/2/1/app-wifi-list.PNG"></img></a></p>
        <p style="text-align:center;"><em>Your Phone should find a network name prefaced with "Photon-", connect to it.</em></p>
        <p>4. Switch back to the Particle app. After a few seconds, the app should present you with a list of WiFi networks. Select the network you want to connect your Photon RedBoard to. On the next screen type in the network's password.
        <p>5. The penultimate screen will show the Photon RedBoard's progress as it connects to your WiFi network and the Particle cloud.</p>
        <p>6. Finally, create a name for your Photon. You can go with Particle's (wonderfully random) suggestion, or one of your own.</p>
        <p style="text-align:center;"><a href="https://cdn.sparkfun.com/assets/learn_tutorials/4/2/1/app-name.PNG"><img src="https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/4/2/1/app-name.PNG"></img></a></p>
      </div>
    </div>
  </div>
  <div class="panel panel-default">
    <div class="panel-heading" role="tab" id="headingTwo">
      <h3 style="text-align:center;" class="panel-title">
        <a class="btn btn-default" class="collapsed" role="button" data-toggle="collapse" data-parent="#accordion" href="#collapseTwo" aria-expanded="false" aria-controls="collapseTwo">
          Configuring WiFi with a Serial Terminal
        </a>
      </h3>
    </div>
    <div id="collapseTwo" class="panel-collapse collapse" role="tabpanel" aria-labelledby="headingTwo" border=1>
      <div class="panel-body">
        <p>If, for some reason or another, you can't use the Particle app to commision your Photon RedBoard, one alternative is using a Serial Terminal.</p>
        <p><div class="alert alert-success">An alternative to this method &ndash; one that doesn't require serial terminal-ing &ndash; is <a href="https://docs.particle.io/guide/tools-and-features/cli/">Particle CLI</a>. After installing particle CLI you can use the <code>particle setup</code> command to run through device setup. More information on that <a href="https://docs.particle.io/guide/tools-and-features/cli/#particle-setup">here</a>.</div></p>
        <p>1. Open a serial terminal program (check out our <a href="https://learn.sparkfun.com/tutorials/terminal-basics">Serial Terminal Basics tutorial</a> for suggestions) to your Photon's serial port. On Windows, the port should look something like COM#. On Mac, the port will be something like /dev/tty.usbmodem####.</p>
        <p>2. Type <b>i</b> to get your Photon's <b>Device ID</b>. Copy it down &ndash; at least temporarily.</p>
        <p>3. Type <b>w</b> to enter <b>WiFi configuration</b>. Then follow along with the prompts to enter you WiFi network's name and password.
        <p style="text-align:center;"><a href="https://cdn.sparkfun.com/assets/learn_tutorials/4/2/1/serial-wifi-enter.PNG"><img src="https://cdn.sparkfun.com/assets/learn_tutorials/4/2/1/serial-wifi-enter.PNG"></img></a></p>
        <p style="text-align:center;"><em>Type 'i' to get your Photon's Device ID, then 'w' to configure its WiFi.</em></p>
        <p>4. Go to the <a href="https://build.particle.io">Particle Build IDE</a>. Create an account if you haven't already, or log in.</p>
        <p>5. Hop over to the "Devices" tab. Select <b>Add New Device</b>, and paste in your Photon RedBoard's Device ID.</p>
        <p style="text-align:center;"><a href="https://cdn.sparkfun.com/assets/learn_tutorials/4/2/1/serial-claim.PNG"><img src="https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/4/2/1/serial-claim.PNG"></img></a></p>
        <p style="text-align:center;"><em>Paste your Photon's Device ID into the Build IDE.</em></p>
        <p>6. Name your new Photon RedBoard
        <p>7. Your Photon RedBoard will initially appear under the "Other" section, but after reloading the IDE, it should find its home under the "P1s" section.</p>
      </div>
    </div>
  </div>
</div>