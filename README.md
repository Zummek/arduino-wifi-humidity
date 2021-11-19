# Humidity sensor

System to collecting data about temperature and humidity in apartment.

### Hardware used

- Humidity sensor - DHT22
- Controller, Wi-Fi - NodeMCU v3 or ESP01 (ESP8266)

### Libraries used

- adafruit/Adafruit Unified Sensor
- adafruit/DHT sensor library
- bblanchon/ArduinoJson

### External service

- Google Scripts

### IDE

- Visual Studio code
- PlatformIO extension

## How is it working?

Controller every X minutes collects data(temperature, relative humidity) from sensor. Next data are serializes to JSON and send to Google Script which process data and insert them as new row in Google Sheet. Data are broken down into months in sheets.

There is a chart in the worksheet that show changes of values over time.

Code is not perfect because is simple. :)

## What to look for?

### Wi-Fi settings - special characters

To use special characters in e.g. SSID, it is necessary to write these characters with Unicode.
E.g: `ó => \u00F3` so SSID: `"tw\u00F3j"`

### Deep sleep

To use this feature, it is necessary to connect some pins. In ESP-01 is a little more complicated. Forr details, look to [this tutorial](https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/) about deep sleep.  
Settings: `DEEP_SLEEP_READY`.

### Led status and serial prints

In ESP-01, internal LED is on the same wire with TX. So is not possible to use internal LED and serial communication in this same time.  
Settings: `DEBUG` and `FORCE_LED_ON`.

## Getting started

- There is a settings file (include/settings.h). Configure them for your own use.
- Create Google Script.
  1. Create new sheet, open sheep.
  2. In Extensions, select Apps Script.
  3. Copy ./googleScript.gs to opened editor.
  4. In first line enter Your Sheet ID got from sheet URL link, then save.
  5. Click Deploy, New deploy, select type: Internet application, change who has access to everyone and click deploy
  6. Copy Deployment ID and past to `GOOLGE_SCRIPT_ID` in `./platformio.ini`.
     > **_NOTE:_** To update save script, select Manage Deployments, pencil, select version to `new version` and click Deploy
- Set Wi-Fi and google script parameters as `build_flags` in `./platformio.ini`.
- If Your platform is not esp01, look to comments in `./platformio.ini`.

## Schematic

To be continued...

## TODO:

- Hide sensitive data from platformio.ini to environment variables.
  - There was a problem with getting env variables over `sysenv` section(platformio)
- Collect X measurements and send them with one request to sheet. Save energy by limiting the use of the Wi-Fi.
  - uC resets on wake up - variables/data are lost. There was an issue with storing measurements in EEPROM between sleep/wake up. EEPROM has a limit of 100,000 write cycles per single location. It may be exhausted after a year.
- Update generating chart to new sheet.
  - There is a bug in Apps Scripts: https://issuetracker.google.com/issues/205506220
