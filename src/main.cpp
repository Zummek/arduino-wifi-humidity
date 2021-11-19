#include <Arduino.h>
#include <DHT.h>
#include "settings.h"
#include "AppWiFi.h"

DHT dht(DHTPIN, DHTTYPE);

uint32_t const measurementDelayMS = 1000UL * 60 * 5; // 5 minutes

float temperature;
float humidity;

void setup()
{
  DEBUG_SERIAL.begin(115200);
  DEBUG_SERIAL.println();
#if IS_LED_ON
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif
  delay(10);
  dht.begin();

  AppWiFi::initWiFi();
  AppWiFi::checkWiFiConnection();

  DEBUG_SERIAL.println("Starting...\n");
  delay(2000);
#if IS_LED_ON
  digitalWrite(LED_BUILTIN, HIGH);
#endif
}

void loop()
{
#if IS_LED_ON
  digitalWrite(LED_BUILTIN, LOW);
#endif

  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity))
  {
    DEBUG_SERIAL.println("Failed to read from DHT sensor!");
    return;
  }
  DEBUG_SERIAL.print(F("\nTemperature: "));
  DEBUG_SERIAL.print(temperature);
  DEBUG_SERIAL.println(F("°C"));
  DEBUG_SERIAL.print(F("Humidity: "));
  DEBUG_SERIAL.print(humidity);
  DEBUG_SERIAL.println(F("%"));

  AppWiFi::sendDataToGoogleScript("appendRow", "" + String(temperature) + ";" + String(humidity));

#if IS_LED_ON
  digitalWrite(LED_BUILTIN, HIGH);
#endif

  delay(measurementDelayMS);
}