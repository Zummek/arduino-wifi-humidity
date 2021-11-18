#include <Arduino.h>
#include <DHT.h>
#include <HTTPSRedirect.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#define DHTPIN 2      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302)

// Debugging - Set to 
// true to enable debug messages over serial port, 
// false to disable and use LED to indicate status
#define DEBUG false
#define FORCE_LED_ON false
#define DEBUG_SERIAL if(DEBUG)Serial
#define IS_LED_ON !DEBUG || FORCE_LED_ON

DHT dht(DHTPIN, DHTTYPE);

const uint32_t measurementDelayMS = 1000UL * 60 * 5; // 5 minutes

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const char* host = "script.google.com";
const char* GScriptId = GOOLGE_SCRIPT_ID;
const int httpsPort = 443;
String url = String("/macros/s/") + GScriptId + "/exec";

StaticJsonDocument<128> payload;
String payloadStr = "";

HTTPSRedirect* client = nullptr;

float temperature;
float humidity;

void setup()
{
  DEBUG_SERIAL.begin(115200);
#if IS_LED_ON
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
#endif
  delay(10);
  dht.begin();

  DEBUG_SERIAL.println();
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
  DEBUG_SERIAL.println("Starting...");
  DEBUG_SERIAL.println();

  delay(2000);
#if IS_LED_ON
  digitalWrite(LED_BUILTIN, HIGH);
#endif
}

void loop()
{
  // Get temperature event and print its value.
#if IS_LED_ON
  digitalWrite(LED_BUILTIN, LOW);
#endif
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  if (isnan(temperature) || isnan(humidity))
  {
    DEBUG_SERIAL.println(temperature);
    DEBUG_SERIAL.println(humidity);
    DEBUG_SERIAL.println("Failed to read from DHT sensor!");
    return;
  }
  DEBUG_SERIAL.println();
  DEBUG_SERIAL.print(F("Temperature: "));
  DEBUG_SERIAL.print(temperature);
  DEBUG_SERIAL.println(F("°C"));
  DEBUG_SERIAL.print(F("Humidity: "));
  DEBUG_SERIAL.print(humidity);
  DEBUG_SERIAL.println(F("%"));

  // send data to google script
  payload["command"] = "appendRow";
  payload["values"] = "" + String(temperature) + ";" + String(humidity);
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

#if IS_LED_ON
  digitalWrite(LED_BUILTIN, HIGH);
#endif
  delay(measurementDelayMS);
}