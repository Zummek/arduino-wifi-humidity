#ifndef APPWIFI_h
#define APPWIFI_h

#include <HTTPSRedirect.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include "settings.h"


class AppWiFi
{
public:
  static void initWiFi();
  static void checkWiFiConnection();
  static void sendDataToGoogleScript(String command, String valuesStr);

private:
  static const char* ssid;
  static const char* password;
  static const char* host;
  static const uint16_t httpsPort;
  static const String url;

  static HTTPSRedirect* client;
  static StaticJsonDocument<128> payload;
  static String payloadStr;
};

#endif
