#include "secrets.h"
#include <WiFi.h>
#include <ESPmDNS.h>

// #include <WebServer.h>
// #include <ArduinoJson.h>
// #include <WebSocketsClient.h>
// #include <ArduinoWebsockets.h>
#include <PubSubClient.h>
#include <HX711.h>
#include <ArduinoOTA.h>

const char *SSID = SECRET_SSID;
const char *PWD = SECRET_PWD;
const char *MQTTUSR = SECRET_MQTTUSR;
const char *MQTTPWD = SECRET_MQTTPWD;
const char *MQTTSERVER = SECRET_MQTTSERVER;
// Device Identifier
const char *DEVICE = "juicebox_1";
// device hostname
String hostname = "juicebox_1";

// Led Pins
const int statusLed = 2;
const int greenLed = 12;
const int blueLed = 14;
// Relay Pins
const int relay1 = 27;
const int relay2 = 26;
const int relay3 = 25;
// Load Cell Pins
const int loadcellDout = 23;
const int loadcellSck = 22;
// mqtt payloads
String weight_str;
char weight[50];

WiFiClient espClient;
PubSubClient mqttClient(espClient);
HX711 scale;
// long lastMsg = 0;
// char msg[50];
// int value = 0;




void connectToWiFi()
{
  WiFi.setHostname(hostname.c_str());;
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
    if (String(topic) == "juicebox1/relay3")
  {
    Serial.print("Changing Relay 3 output to ");
    if (messageIn == "on")
    {
      Serial.println("on");
      digitalWrite(relay3, LOW);
    }
    else if (messageIn == "off")
    {
      Serial.println("off");
      digitalWrite(relay3, HIGH);
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
      mqttClient.subscribe("juicebox1/relay1");
      mqttClient.subscribe("juicebox1/relay2");
      mqttClient.subscribe("juicebox1/relay3");
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
    // long reading = scale.read();
    long reading = scale.get_units(15);
    String readingTempString = String(reading);
    char readingMessage[50];
    readingTempString.toCharArray(readingMessage, readingTempString.length() + 1);
    // String messageToSend = String("{\"tankWeight\": \" "readingString" \" }");
    mqttClient.publish("juicebox1/tankWeight", readingMessage);
    Serial.print("HX711 reading: ");
    Serial.println(reading);
  } else {
    Serial.println("HX711 not found :(");
  }

  // delay(1500);
}

void setup()
{
  // pin setup
  // relays
  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, HIGH); // !start relay1 HIGH (Which is off????)
  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, HIGH); // !start relay1 HIGH (Which is off????)
  pinMode(relay3, OUTPUT);
  digitalWrite(relay3, HIGH); // !start relay1 HIGH (Which is off????)
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
  // calibration
  scale.set_scale(-22.3);
  scale.set_offset(-576184);
  // scale.tare();

  // OTA Setup
  ArduinoOTA.setHostname("juicebox_1");
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{
  if (!mqttClient.connected())
  {
    reconnect();
  }
  printScales();
  ArduinoOTA.handle();
  mqttClient.loop();
     //not using - will try again later!
}