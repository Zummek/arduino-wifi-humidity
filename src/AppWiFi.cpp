#include "AppWiFi.h"

const char* AppWiFi::ssid = WIFI_SSID;
const char* AppWiFi::password = WIFI_PASSWORD;
const char* AppWiFi::host = "script.google.com";
const uint16_t AppWiFi::httpsPort = 443;
const String AppWiFi::url = String("/macros/s/") + GOOLGE_SCRIPT_ID + "/exec";

StaticJsonDocument<128> AppWiFi::payload;
String AppWiFi::payloadStr = "";

HTTPSRedirect* AppWiFi::client = nullptr;

void AppWiFi::initWiFi() {
  DEBUG_SERIAL.print("Connecting to wifi: ");
  DEBUG_SERIAL.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    DEBUG_SERIAL.print(".");
  }
  DEBUG_SERIAL.println("");
  DEBUG_SERIAL.println("WiFi connected");
  DEBUG_SERIAL.print("IP address: ");
  DEBUG_SERIAL.println(WiFi.localIP());
}

void AppWiFi::checkWiFiConnection() {
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setContentTypeHeader("application/json");
  bool flag = false;
  for (int i = 0; i < 5; i++)
  {
    int retval = client->connect(host, httpsPort);
    if (retval == 1)
    {
      flag = true;
      break;
    }
    else
      DEBUG_SERIAL.println("Connection failed. Retrying...");
  }

  if (!flag)
  {
    DEBUG_SERIAL.print("Could not connect to server: ");
    DEBUG_SERIAL.println(host);
    DEBUG_SERIAL.println("Exiting...");
    return;
  }
}

void AppWiFi::sendDataToGoogleScript(String command, String valuesStr) {
  payload["command"] = command;
  payload["values"] = valuesStr;
  payloadStr = "";
  serializeJson(payload, payloadStr);

  if (client == nullptr)
  {
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }

  if (!client->connected())
    client->connect(host, httpsPort);

  client->POST(url, host, payloadStr, false);

  if (client->getResponseBody().compareTo("Success\r\n") == 0) {
    DEBUG_SERIAL.println(F("Data sent to Google Script"));
  }
  else
  {
    DEBUG_SERIAL.print("Payload: ");
    DEBUG_SERIAL.println(payloadStr);
    DEBUG_SERIAL.println("Error sending data to Google Script");
    DEBUG_SERIAL.println(client->getResponseBody());
  }

  // Disconnect the client from the server when intervals between requests is too long
  // Library don't react on long intervals
  client->stop();
  payload.clear(); // clear payload for next request (some errors can occur if not)
}