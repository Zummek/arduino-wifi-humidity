# Humidity sensor

System to collecting data about humidity in apartment.

### Hardware used

- Humidity sensor - DHT22
- Controller, Wi-Fi - NodeMCU v3 (ESP8266) [wanna try with ESP01]

### Libraries used

- adafruit/Adafruit Unified Sensor
- adafruit/DHT sensor library
- bblanchon/ArduinoJson

### External service

- Google Scripts

## How it's working?

Controller every X minutes collects data(temperature, relative humidity) from sensor. Next data are serializes to JSON and send to Google Script which process data and insert them as new row in Google Sheet.

There is a chart in the worksheet that show live change of values over time.

Code is not perfect because is simple. I didn't have to use, among others timers.

## TODO:

- Hide sensitive data from platformio.ini to environment variables.
  - There was a problem with getting env variables over `sysenv` section(platformio)
- ...
