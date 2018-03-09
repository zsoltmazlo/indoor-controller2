# Custom smart-home sytem

This project is part of my very own smart-home system built in my home to control devices across my apartment and measure temperature both indoor and outdoor. This system currently consist 6 working units:

* A central unit serving several python-based microservices and running an Angular-based UI on a 7" touchscreen. 
* An indoor unit controlling two led-strips over my desk and measuring temperature in my room every 10 minutes.
* An indoor unit controlling an RGB led-strip over my bed.
* An indoor unit controlling my TV replaying the control-messages recorded from my TV-remote.
* An indoor unit to measure temperature in the Kitchen (currently out of service).
* An outdoor unit to measure temperature (currently out of service).

# Indoor controller based on Particle Photon

This unit is responsible for one task: to control the RGB led-strip over my bed.


## Hardware

The unit is based on a Particle Photon module with three potmeters as input and three IRLZ44N MOSFET to dim the intensity of the led-strips. The unit uses 12V input to power both the LED-strips and the controller (a small DC-DC step-down converter included). I had designed a custom box for the device and printed out with our 3D printer. 

The unit uses a 2.4" TFT display to show current information and also, there is a small switch to switch on/off the display.

![controller unit](https://raw.githubusercontent.com/wiki/zsoltmazlo/indoor-controller1/2017-08-23%2019.08.54.jpg)


## Software

The firmware for the controller is developed with Netbeans using the Particle's official [firmware repository](https://github.com/particle-iot/firmware) as base. The communication in the whole system is based on MQTT, thus the firmware uses the [hirotakaster's MQTT implementation](https://github.com/hirotakaster/MQTT) to communicate with other modules - although I have implemented a wrapper class around the MQTT class to use C++11 features like lambda functions etc, but the previous version was used a plain-old TCP server socket which implementation is still in the Connection class.

The display is controlled using the Adafruit GFX library, but I have implemented a custom class to use custom command in my code.

## Usage

You should able to build the respository simply issuing a `make` command, and to upload this image to a Particle unit, you should issue the `make program-dfu PARTICLE_DEVELOP=1 PLATFORM=photon USE_SWD_JTAG=n MODULAR=n` command. 