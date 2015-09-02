### Introduction

This experiment will get you (or at least your Photon) moving, because we'll be wiring up a motion-sensing, orientation-finding, [3-axis accelerometer](https://www.sparkfun.com/products/12756). Hardware-wise, this experiment serves as an introduction to accelerometers and I<sup>2</sup>C communication. You'll also learn about **system modes**, which allow you to turn WiFi on or off and save loads of battery juice.

#### Parts Needed

* **1x** MMA8452Q Accelerometer
* **1x** Green Button
* **6x** Jumper wires

<!-- products_by_id(12795, 12756, 10302) -->

#### Suggested Reading

Two big new electronics concepts are introduced in this experiment: [Accelerometers](tutorials/63) and [I<sup>2</sup>C Communication](tutorials/82). If you're interested in learning a lot more about them, check out our tutorials. For now, here's a quick introduction:

####Accelerometers

**Accelerometers** are movement sensors. They _feel_ acceleration -- the rate of change of velocity (how fast something is speeding up or slowing down). They've become incredibly common throughout consumer electronics, with a huge range of applications: smartphone's use them to sense orientation, activity trackers often track steps using an accelerometer, and hard drives use them to sense free-fall (giving them enough time to move and protect delicate parts).

Even if a device isn't visibly moving, an accelerometer can still give you a lot of information about its orientation. Accelerometers sense the acceleration of gravity, which is a constant, pulling force toward earth. In fact, one of the most common **units of acceleration** is the _**g**_ -- "gravity" -- which is equal to about 9.8 <em>m/s<sup>2</sup></em>. An accelerometer sitting flat and motionless, will sense 1<em>g</em> of acceleration towards the ground (usually on the z-axis), and 0<em>g</em> of acceleration in the other two dimensions (x and y).

-> [![MMA8452Q axis orientations](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/activity-accelerometer-axes.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/activity-accelerometer-axes.png) <-

There are a huge variety of accelerometers out there. They can monitor anywhere from 1-to-3 axes of acceleration, support various communication interfaces, and host a range of other unique features. In this experiment, we're using the [MMA8452Q](https://www.sparkfun.com/products/12756) -- a **3-axis** accelerometer with a digital interface. It can be set to sense a maximum of either &plusmn;2, 4, or 6 _g_. It also supports a neat feature called tap, or "pulse" detection.

#### I2C

The accelerometer we're using in this experiment communicates over an interface called **I<sup>2</sup>C**, short for <em>I</em>nter [<em>I</em>C](tutorials/80) <em>C</em>ommunication. I<sup>2</sup>C is an extremely popular embedded computing communication interface. It's relatively slow (the Photon and accelerometer communicate at about 400kHz), but only requires **two wires** for communication. These wires are usually called serial data (**SDA**) and serial clock (**SCL**). As you'll see in the next section, hooking up I<sup>2</sup>C circuits is as easy as connecting SDA-to-SDA and SCL-to-SCL (don't forget to power the device too!).

Devices on an I<sup>2</sup>C bus are called "slaves", while the lone, controlling component (our Photon RedBoard in this case), is called the "master". What makes I<sup>2</sup>C even more powerful is it allows multiple slave devices on a single, two-wire bus. Want to add a [pressure sensor](https://www.sparkfun.com/products/9721) to your circuit? Just connect the SDA's and SCL's. Each slave device on an I<sup>2</sup>C bus has a unique address, so they can ignore messages for other devices, and only take action on bytes meant for them.

### Hardware Hookup

Here's the hookup for both parts of this experiment:

-> [![Activity Track fritzing hookup](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/activity-tracker-fritzing_bb.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/activity-tracker-fritzing_bb.png) <-

As we mentioned in the I<sup>2</sup>C section above, an I<sup>2</sup>C bus is made up of two signals: SDA and SCL. An I<sup>2</sup>C interface also requires pullup-resistors on those signals, but the breakout board already takes care of that for you.

The button signal is routed to the RedBoard's D3 pin. We'll internally pull that pin up, so when the button is inactive it'll read HIGH. When the button is pressed, D3 will go LOW. We won't use the button in the first part, but it'll come in handy later on.
	   
### Photon Code

In this experiment, we'll once again be using the Libraries feature of the Build IDE. To communicate with the accelerometer, we'll be using the **SparkFunMMA8452Q** library.

This time, however, we'll be using another feature of the Libraries tab: **examples**. Most libraries include at least one example, to help demonstrate their features and usage. To use the SparkFunMMA8452Q library's example, click **USE THIS EXAMPLE** (make sure the "mma8452q-serial-example.ino" tab is active up top).

-> [![Using a library example](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/activity-tracker-library-example.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/activity-tracker-library-example.png) <-

The Build IDE will create a new app in your code section called **MMA8452Q-Serial-Example**, and it will already have the library included for you.

All you have to do is flash the code!

### Code to Note

This example introduces the SparkFunMMA8452Q library. To begin using the library, create an `MMA8452Q` class object. This'll often go in the global section of the code:

    language:c
    // Create an MMA8452Q object, used throughout the rest of the sketch.
    MMA8452Q accel; // Default constructor, SA0 pin is HIGH

We'll use `accel` from here on to access the accelerometer functions and variables. To initialize the sensor, stick `accel.begin()` in your `setup()`. You can give the `begin()` function two parameters, if you want to configure the **sensing range** of the accelerometer or the **output data rate (ODR)**.

    language:c
    // Initialize the accelerometer with begin():
    // begin can take two parameters: full-scale range, and output data rate (ODR).
    // Full-scale range can be: SCALE_2G, SCALE_4G, or SCALE_8G (2, 4, or 8g)
    // ODR can be: ODR_800, ODR_400, ODR_200, ODR_100, ODR_50, ODR_12, ODR_6 or ODR_1
    accel.begin(SCALE_2G, ODR_1); // Set up accel with +/-2g range, and slowest (1Hz) ODR

The scale can be set to either &plusmn;2, 4, or 8 _g_, while the output data rate can be set to either 1, 6, 12, 50, 100, 200, 400, or 800 Hz. In our example, the scale is set to its minimum -- meaning it'll have the highest resolution, but be limited to sensing a maximum of 2 _g_.

To check if new data is available from the sensor, use the `accel.available()` function, which will return 1 if there's data to be read or 0 otherwise.

Once new data is available, reading and using data from the accelerometer is a two-step process. First, call `accel.read()` to get the data. Then you'll be able to access any of six class variables: x, y, z -- the "raw" 12-bit values from the accelerometer -- and cx, cy, and cz, the calculated accelerations in g.

The whole process goes something like this:

    language:c
    if (accel.available())
    {
        accel.read();
    
        Serial.println("X: " + String(accel.x) + " | " + String(accel.cx, 2) + " g");
        Serial.println("Y: " + String(accel.y) + " | " + String(accel.cy, 2) + " g");
        Serial.println("Z: " + String(accel.z) + " | " + String(accel.cz, 2) + " g");
    }

### What You Should See

This part of the experiment is designed to get you familiar with what, exactly, an accelerometer senses. We'll be reading the acceleration from each of the three axes, then printing those values out to the **serial monitor**.

Once the Photon begins running the application, open up a serial terminal to view the data. Acceleration sensed on all three axes is displayed at a rate of about 1Hz. There are also some fancy ASCII bar graphs, to visually represent the acceleration values.

-> [![Accelerometer activity printed to serial monitor](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/activity-tracker-serial.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/activity-tracker-serial.png) <-

Sitting flat, you should see about 1 _g_ of acceleration on the z-axis, and nearly 0 _g_ on the x- and y-axes. Carefully -- without disturbing any of the wires -- tilt your RedBoard and Breadboard around, and monitor the three accelerometer readings. If you flip the board 90&deg;, the z-axis should become 0 _g_ and either x or y will go to 1. Can you tilt the board and get each axis nearly equal?

### Troubleshooting

Motion and breadboard/jumper circuits aren't usually a great combination. If your circuit works initially, but mysteriously stops working, a jumper may have been (even briefly) disconnected. Double-check all of your wiring, and restart if anything stops working. Move things around carefully! The base plate is extremely useful for this experiment.

##Part 2: Tracking Steps, Publishing Your Acitvity

Accelerometers are commonly used as the motion-sensing foundation of pedometers -- step counters. And while the baseplate probably isn't the most comfortable thing to strap to your belt and walk around with, creating a activity monitor of our own can be a fun exercise. Plus it provides an algorithm you can endlessly try to tweak and perfect.

### Suggested Reading

**[Photon System Modes](https://docs.particle.io/reference/firmware/photon/#system-modes)** are used in this experiment to control the Photon RedBoard's connection to WiFi and the Particle cloud. By placing a `SYSTEM_MODE(MANUAL)` definition at the top of the sketch, we've told the Photon that we want complete control over the state of WiFi.

Our Photon RedBoard Activity Monitor is, of course, going to be battery powered. Turning WiFi off when it's not needed, is a great way to save loads of battery life by reducing the module's power consumption. It takes some extra planning to use `MANUAL` mode, but can result in a big payoff.

* **[Particle Publish](https://docs.particle.io/reference/firmware/photon/#spark-publish-)** will be our tool for sharing our step activity with the world. Publish can be used to post named event data to the Particle Cloud. Where the data can be grabbed by another application and displayed or used otherwise.

The [Particle Dashboard](https://dashboard.particle.io) is an easy tool for viewing your device's published data. In leiu of anything more complex, we can use that interface to easily monitor the number of steps our RedBoard has taken.

### New Photon Code

Create a new application, and post this code in:

Grab the code from [Code/02-ActivityPublisher/ActivityPublisher.ino](https://github.com/sparkfun/Inventors_Kit_For_Photon_Experiments/blob/master/08-ActivityTracker/Code/02-ActivityPublisher/ActivityPublisher.ino).

As with the previous part's code, you'll need to include the **SparkFunMMA8452Q** library in this application.

### Code to Note

#### Publishing Events and Data

To share our data with the world, we'll use Particle's publish feature. Publishing allows us to alert the outside world that an "event" happend, and even post some data value under that event.

In this example, we're publishing our step data under the "Steps" event name, and including the step count under that data. Just a few lines of code are required to publish, and verify a successful publish:

	language:c
	// Call Particle.publish to push our step count to the web:
    publishSuccess = Particle.publish("Steps", String(stepCount / 2));
    
    // If the publish was successful
    if (publishSuccess)
    {
        publishFlag = false; // clear the publishFlag
        stepCount = 0; // and reset the step count
    }

Once you've published your data to the Cloud, there should be no shortage of actions you can take with it. You can set up a webhook, to monitor the event and post its data. Or even configure a second Photon to [Subscribe](https://docs.particle.io/reference/firmware/photon/#spark-subscribe-) to the event, to track multiple Photon activity monitors.

#### System Modes, and Controlling WiFi

The Photon's WiFi component is a relatively huge power-consumer (consuming on average about !!!mA). That may not be a big deal when it's powered over USB, but when we're operating on battery power, every miliwatt (mW) means less money going toward replacing batteries. So in this experiment we turn off WiFi whenever it's not necessary -- you don't need WiFi to track steps!

Control of the Photon's WiFi connection begins before `setup()`, by calling the [`SYSTEM_MODE` macro](https://docs.particle.io/reference/firmware/photon/#system-modes):

	language:c
	// SEMI_AUTOMATIC mode starts the application up without WiFi, not connected to the
	// Particle cloud. Call Particle.connect() manually to connect to WiFi, after that
	// Particle.process()'s will be handled for us.
	SYSTEM_MODE(SEMI_AUTOMATIC); // Semi-automatic WiFi/cloud mode

`SYSTEM_MODE` can either be `AUTOMATIC`, `SEMI_AUTOMATIC` or `MANUAL`. `AUTOMATIC` is the default state, which you should be extremely familiar with by now. The Photon boots up and immediately tries to connect to WiFi and the Cloud before running the sketch.

In `SEMI_AUTOMATIC` mode, our Photon jumps straight into our application code. It won't try to connect to the cloud until we call `Particle.connect()`, and `Particle.disconnect()` can be called to disconnect. While the device is connected, its connection with the cloud is automatically administered.

`MANUAL` mode leaves everything -- including connecting and cloud-processing -- to the user. After connecting to the cloud, `Particle.process()` still has to be called to maintain the cloud connection.

In addition to the `SYSTEM_MODE` control, the application also manages our WiFi connection with `WiFi.on()`, `WiFi.connect()`, and `WiFi.off()`. For example, after the button has been pressed, we tell the Photon RedBoard to connect to WiFi and the Particle cloud like this:

	language:c
	WiFi.on(); // Turn the WiFi module on
	WiFi.connect(); // Connect to the pre-set WiFi SSID
	Particle.connect(); // Connect to the Particle Cloud
	
And once we've successfully published, `WiFi.off()` is called to shut the WiFi system down.

#### Sensing Steps

Before we can publish any step count, we have to sense them. We'll be honest here: pedometer algorithm's are tough. Luckily, the MMA8452Q has a neat feature that we can pigeonhole into our application:  pulse detection. We can set the accelerometer to continuously monitor all three axes for short pulses of motion on any of the three axes. All we need to do is tell it what **threshold** of acceleration we want to trigger on, the maximum duration of a pulse, and the minimum time between pulses.

To setup pulse or "tap" detection on the MMA8452Q, use the `setupTap([thsX], [thsY], [thsZ], [maxDuration], [minWindow])` function. We'll set the threshold to be very low, so just about any movement will create a tap, and set the time window between pulses to about 500ms (how fast do you walk?).

    accel.setupTap(threshold, threshold, threshold, pulseTimeLimit, pulseLatency);

Everyone has a different gait, so you may have to tweak these values to get a more accurate step count.

### What You Should See

After flashing code to your Photon, remove the USB cable, and plug in a 9V battery via the included 9V-to-Barrel Jack adapter.

-> `Image activity monitor powered by battery` <-

Before walking away, open up the [Particle Dashboard](https://dashboard.particle.io), and click over to the "Logs" tab. It should be empty for now.

Then, strap your circuit to your belt, or just hold it in your hand, and take about 50 steps. When you want to publish your step count, hit the red button. Your Photon will stop tracking steps while it connects to WiFi and the Particle Cloud, then posts the step count. When the activity publishes, you should see a new row-entry on the dashboard.

-> [![Particle Dashboard](https://cdn.sparkfun.com/r/600-600/assets/learn_tutorials/3/3/2/activity-tracker-particle-dashboard-sceen.png)](https://cdn.sparkfun.com/assets/learn_tutorials/3/3/2/activity-tracker-particle-dashboard-sceen.png) <-

After successfully publishing, the Photon RedBoard shuts of WiFi and goes back into step counting mode.

### Troubleshooting

Saving power is great, but it can lead to some headaches if you need to upload new code to the Photon. If your Photon's WiFi is off, or if it's not connected to the cloud, you won't be able to flash new code to it. Thankfully, there's **Safe Mode**. By booting your Photon into safe mode, it skips running your application and instead connects to the Particle Cloud and waits for a new program.

To boot into safe mode, hold down both the **RESET** and **MODE** buttons. Then release _RESET_ to turn the Photon on. When the RGB LED begins **blinking pink-ish**, release _MODE_. Your Photon will run through its WiFi/Cloud connection process, then breathe pink. Once it's breathing pink, you'll be able to successfully flash new code to it.