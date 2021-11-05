#include <Arduino.h>
#include <DHT.h>
#include <HTTPSRedirect.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#define DHTPIN 5      // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22 // DHT 22 (AM2302)

DHT dht(DHTPIN, DHTTYPE);

uint32_t sampleDelayMS = 1000UL * 60 * 5; // 5 minutes

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *host = "script.google.com";
const char *GScriptId = GOOLGE_SCRIPT_ID;
const int httpsPort = 443;
String url = String("/macros/s/") + GScriptId + "/exec";

StaticJsonDocument<128> payload;
String payloadStr = "";

HTTPSRedirect *client = nullptr;

float temperature;
float humidity;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.begin(115200);
  delay(10);
  dht.begin();

  Serial.println();
  Serial.print("Connecting to wifi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

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
      Serial.println("Connection failed. Retrying...");
  }

  if (!flag)
  {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }
  Serial.println("Starting...");
  Serial.println();

  delay(2000);
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop()
{
  // Get temperature event and print its value.
  digitalWrite(LED_BUILTIN, LOW);
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  if (isnan(temperature) | isnan(humidity))
  {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.println();
  Serial.print(F("Temperature: "));
  Serial.print(temperature);
  Serial.println(F("°C"));
  Serial.print(F("Humidity: "));
  Serial.print(humidity);
  Serial.println(F("%"));

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

  if (client->getResponseBody().compareTo("Success\r\n") == 0)
    Serial.println(F("Data sent to Google Script"));
  else
  {
    Serial.print("Payload: ");
    Serial.println(payloadStr);
    Serial.println("Error sending data to Google Script");
    Serial.println(client->getResponseBody());
  }

  // Disconnect the client from the server when intervals between requests is too long
  // Library don't react on long intervals
  client->stop();
  payload.clear(); // clear payload for next request (some errors can occur if not)

  digitalWrite(LED_BUILTIN, HIGH);
  delay(sampleDelayMS);
}