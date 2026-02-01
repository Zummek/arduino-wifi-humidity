#ifndef SETTINGS_h
#define SETTINGS_h

#define DHTPIN D1      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302)

#define DEEP_SLEEP_READY false

// Debugging - Set to 
// true to enable debug messages over serial port, 
// false to disable and use LED to indicate status
#define DEBUG true
#define DEBUG_SERIAL if(DEBUG)Serial
// If You can use internal LED with serial communcation, set to true
// Problem is with ESP-01 where internal led is on the same wire with TX
#define FORCE_LED_ON false
#define IS_LED_ON !DEBUG || FORCE_LED_ON

#define MEASUREMENT_DELAY_MS 1000UL * 60 * 5 // 5 minutes

#endif