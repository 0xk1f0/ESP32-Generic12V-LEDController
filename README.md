# ESP32-Generic12V-LEDController
Control Generic 12V Infrared LED-Strips with an ESP32

## How To Use

1. Clone this Repository
2. Open it in VSCode with PlatformIO installed
3. Change WiFi-SSID, WiFi-Password and Hostname in `include/settings_template.h`
4. If necessary, also change the Pin-Layout according to your wiring
4. Rename settings_template.h to settings.h
5. Compile it
6. Flash it

## Updating via OTA-Webpage

1. Navigate to Settings->OTA-Update or go to "http://[ESP32-IP]/update"
2. Click on the file input and select your updated binary file
3. Click "Start Update!" and wait (don't close the page!)
4. If the update was successful, you will be redirected automatically

## Features

- Web-Interface with Color Sliders and ON/OFF-Button
- Fading effects on state change
- OTA-Update functionality

## To Be Added

- Color Wheel
- Effects
- Prettier Web-Interface
- JSON-API
- WLED-App Compatibility
