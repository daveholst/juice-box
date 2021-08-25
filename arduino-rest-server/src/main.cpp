#include "secrets.h"
#include <WiFi.h>
// #include <WebServer.h>
// #include <ArduinoJson.h>
// #include <WebSocketsClient.h>
// #include <ArduinoWebsockets.h>
#include <PubSubClient.h>
#include <HX711.h>

const char *SSID = SECRET_SSID;
const char *PWD = SECRET_PWD;
const char *MQTTUSR = SECRET_MQTTUSR;
const char *MQTTPWD = SECRET_MQTTPWD;
const char *MQTTSERVER = SECRET_MQTTSERVER;
// Device Identifier
const char *DEVICE = "juicebox_1";

// Led Pins
const int statusLed = 2;
const int greenLed = 12;
const int blueLed = 14;
// Relay Pins
const int relay1 = 27;
const int relay2 = 26;
// Load Cell Pins
const int loadcellDout = 23;
const int loadcellSck = 22;


WiFiClient espClient;
PubSubClient mqttClient(espClient);
HX711 scale;
// long lastMsg = 0;
// char msg[50];
// int value = 0;

void connectToWiFi()
{
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PWD);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
    // we can even make the ESP32 to sleep
  }

  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageIn;

  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageIn += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "juicebox1/relay1")
  {
    Serial.print("Changing Relay 1 output to ");
    if (messageIn == "on")
    {
      Serial.println("on");
      digitalWrite(relay1, LOW);
    }
    else if (messageIn == "off")
    {
      Serial.println("off");
      digitalWrite(relay1, HIGH);
    }
  }
  if (String(topic) == "juicebox1/relay2")
  {
    Serial.print("Changing Relay 2 output to ");
    if (messageIn == "on")
    {
      Serial.println("on");
      digitalWrite(relay2, LOW);
    }
    else if (messageIn == "off")
    {
      Serial.println("off");
      digitalWrite(relay2, HIGH);
    }
  }
}
void reconnect()
{
  // Loop until we're reconnected
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(DEVICE, MQTTUSR, MQTTPWD))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      mqttClient.publish("juicebox1", "hello world");
      // ... and resubscribe
      mqttClient.subscribe("juicebox1/#");
      // mqttClient.subscribe("juicebox1/relay2");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void printScales()
{
  if (scale.wait_ready_timeout(1000)) {
    long reading = scale.read();
    Serial.print("HX711 reading: ");
    Serial.println(reading);
  } else {
    Serial.println("HX711 not found :(");
  }

  delay(1500);
}

void setup()
{
  // pin setup
  // relays
  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, HIGH); // !start relay1 HIGH (Which is off????)
  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, HIGH); // !start relay1 HIGH (Which is off????)

  //leds
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  Serial.begin(9600);

  connectToWiFi();
  // setup MQTT client
  mqttClient.setServer(MQTTSERVER, 1883);
  mqttClient.setCallback(callback);

  // start scales
  scale.begin(loadcellDout, loadcellSck);
}

void loop()
{
  if (!mqttClient.connected())
  {
    reconnect();
  }
  mqttClient.loop();
  printScales();
}