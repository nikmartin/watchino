# watchino
An ESP32-C6 based smartwatch running Arduino. [Running on a Waveshare esp32-c6 SmartWatch](https://www.waveshare.com/ESP32-C6-Touch-AMOLED-2.06.htm) dev platform. UI by LVGL. 

Libraries used:
* lvgl 9.5
* Arduino_GFX
* SensorLib
* XPowersLib
* A few more Waveshare provided libs

This watch doesnt do anything fancy (yet), It essentially gets the time from my phone's BLE CTS (Current Time Service) and that's it.

### Planned features
* Use the 6 axis gyro to count steps
* BLE connectivity to the OBD2 dongle in my vehicle
* Connect to home automation to turn lights on and off
* SMS and Email notifications


