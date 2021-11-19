#ifndef SETTINGS_h
#define SETTINGS_h

#define DHTPIN 2      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302)

// Debugging - Set to 
// true to enable debug messages over serial port, 
// false to disable and use LED to indicate status
#define DEBUG true
#define FORCE_LED_ON false
#define DEBUG_SERIAL if(DEBUG)Serial
#define IS_LED_ON !DEBUG || FORCE_LED_ON

#endif