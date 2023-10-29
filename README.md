# the-clock--firmware-esp32-idf
A firmware written for the Esp32 microcontroller with the Idf toolchain, to implement a clock synchronized with the network time.

Requires a Wifi router with WPS (push-button), and connected to the internet. The user MUST have access to that WPS push-button. The user MUST have access to the administration interface of the router too.

# Quickly written manual

This is a small bullet point to, hopefully, allow one to build a working firmware and get something on compatible hardware.

## What to expect when it works ?

* After each power on, the device MUST be enrolled to your wifi network using the WPS push button of your router. Pushing the reset button of the device then the WPS push-button should give you enough time to succeed.
* Once connected to the Internet through the Wifi router, the clock get its time from a NTP server (done only once for now).
* The clock displays the time of Paris, France (for now the timezone is hardcoded)
* Between 20:00 and 8:00, the display's brightness is reduced.

## The hardware part 

### pseudo-BOM

* An ESP32 development board with lots of IOs availables ; I am working with a "Lolin32 lite".
* A TM1637-based digital clock display (4 digits, with a colon activated with the decimal point of the second digit from the left, decimal point of all the digit stay dark), with 2 1k pull-up resistors for the IIC connection
* Some push-buttons (not used yet, I started with 3 and plan to have 4), with their 5k1 pull-up resistors
* some wires and your system of choice to assemble stuff

### Assembly

The mentionned IOs are those that I use. The menuconfig allow to change them to whatever GPIO pin that one see fit.
* TM1637: SCLK -- GPIO 32 ; SDAT -- GPIO 33 ; VCC -- 3v3 ; GND -- GND.
* Push-buttons: 'MENU' -- GPIO 15 ; 'DOWN' -- GPIO 16 ; 'UP' -- GPIO 17

## The software part

I am working on VS code, with PlatformIO. I will assume that you know how to use them, even if I try to disambiguates some statements by describing the buttons to click.

Clone the repository somewhere. PlatformIO will download stuff when adding the project to your workspace. It can be big if it has to retrieve the ESP32 IDF development platform, otherwise just a bunch of small libraries.

### Before trying to compile

Open a platformio shell, and run menuconfig with `pio run -t menuconfig`

* You need to activate WIFI, WPS, NTP. If there is something else missing, the build will fails and hopefully tells what is missing. As a last hope, the `sdkconfig.lolin32_lite` is ok, look into that.
* There is a dedicated section _The Clock by Sporniket_ : set up the greetings message, the sntp server, and also the button mapping and IIC port configuration. The default values are ok if you have the same wiring as me.

### Build and install

* Optionnaly, do a clean first (the PIO's trash icon in the status bar).
* Don't forget to plug your ESP32 board through USB.
* Click on PIO's «Upload» button in the status bar. It should start compiling and then programming the board.
* Leave the board plugged, for now.

### Post installation steps

* Open a PlatformIO serial terminal (the PIO's plug icon in the status bar)
* Push the reset button of the ESP32, then push the WPS button of the router. The logs should mention if it succeed to get enrolled in the network.
* If your router is blocking unauthorized devices, connect to the administration interface, spot your device (the logs mention the MAC address) and authorize it.
* Again, push the reset button of the ESP32, then push the WPS button of the router. The logs should tell that it succeeded to connect to wifi, and to get time from an NTP server.

_Everytime the power is interrupted_

* Push the reset button of the ESP32, then push the WPS button of the router. (Memorizing Wifi credentials that it got from WPS is on my roadmap, but not today)
* Now hopefully, your clock is synchronized and running, enjoy !

# The roadmap

Now that the project demonstrate that it can retrieve the time, I plan to have the following features : 

* Remembering the Wifi credentials obtained through WPS.
* Regularly check the time with the NTP server
* Use the buttons to check the time or retry to connect to the wifi or whatever.
* Add a RTC module to keep the time after a power failure, or when there is no wifi
* Add a BMS to wait between power outage -- and save some power
* Add an alarm-clock functions
* Add an MP3 player and have a speaking alarm-clock
* ...