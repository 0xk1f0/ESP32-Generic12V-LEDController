# ESP32-Generic12V-LEDController
Control Generic 12V Infrared LED-Strips with an ESP32

## How To Use

1. Clone this Repository
2. Open it in VSCode with PlatformIO installed
3. Change WiFi-SSID, WiFi-Password and Hostname in `include/settings_template.h`
4. If necessary, also change the Pins Layout according to your wiring
4. Rename settings_template.h to settings.h
5. Compile it
6. Flash it

## Features

- Web-Interface with Color Sliders and ON/OFF-Button
- Fade in/out when toggling the LED's Power state
- OTA-Update functionality (located at "http://YOURIPADDRESS/update")

## To Be Added

- Color Wheel
- Effects
- Prettier Web-Interface