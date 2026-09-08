# Watchino

An ESP32-C6 based smartwatch running Arduino. [Running on a Waveshare esp32-c6 SmartWatch](https://www.waveshare.com/ESP32-C6-Touch-AMOLED-2.06.htm) dev platform. UI by [LVGL](https://lvgl.io/) + [EEZ Studio](https://www.envox.eu/studio/studio-introduction/). Communication with Android phone via [Gadgetbridge](https://gadgetbridge.org)

Libraries used:

* lvgl 9.5
* Arduino_GFX
* SensorLib
* XPowersLib
* A few more Waveshare provided libs

This watch doesn't do anything fancy yet, It essentially gets the time from my phone via GadgetBridge by emulating a Bangle.js smartwatch and that's it.

### Planned features

- [ ] Use the 6 axis IMU to count steps (in progress)
- [ ] BLE connectivity to the OBD2 dongle in my vehicle
- [ ] Connect to home automation to turn lights on and off
- [ ] SMS and Email notifications
- [ ] Power management (how long can I make the battery last?)


