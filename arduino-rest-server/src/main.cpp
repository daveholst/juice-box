#include "secrets.h"
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

const char *SSID = SECRET_SSID;
const char *PWD = SECRET_PWD;
const char *MQTTUSR = SECRET_MQTTUSR;
const char *MQTTPWD = SECRET_MQTTPWD;
const char *MQTTSERVER = SECRET_MQTTSERVER;
// Device Identifier
const char *DEVICE = "juicebox_2";
// device hostname
String hostname = "juicebox_2";

// Led Pins
// const int statusLed = 2;
// const int greenLed = 12;
// const int blueLed = 14;

// Relay Pins
const int relay8 = 5, relay7 = 4, relay6 = 0, relay5 = 15;
const int relay4 = 13, relay3 = 12, relay2 = 14, relay1 = 16;

WiFiClient espClient;
PubSubClient mqttClient(espClient);
// HX711 scale;
// long lastMsg = 0;
// char msg[50];
// int value = 0;

void connectToWiFi()
{
  WiFi.setHostname(hostname.c_str());
  ;
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, PWD);

  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }

  Serial.print("Connected. IP: ");
  Serial.println(WiFi.localIP());
}

bool checkStringIsNumerical(String myString)
{
  uint16_t Numbers = 0;

  for (uint16_t i = 0; i < myString.length(); i++)
  {
    if (myString[i] == '0' || myString[i] == '1' || myString[i] == '2' || myString[i] == '3' || myString[i] == '4' || myString[i] == '5' ||
        myString[i] == '6' || myString[i] == '7' || myString[i] == '8' || myString[i] == '9')
    {
      Numbers++;
    }
  }

  return Numbers == myString.length() ? true : false;
}

void callback(char *topic, byte *message, unsigned int length)
{
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageIn;
  // Generate/read the message - not sure if this is required
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)message[i]);
    messageIn += (char)message[i];
  }
  // Check if it is a relay control message
  if (String(topic).endsWith("/fert1"))
  {
    Serial.print("Changing Fertiliser 1 output to ");
    if (messageIn == "on")
    {
      Serial.println("on");
      digitalWrite(relay1, HIGH);
    }
    else if (messageIn == "off")
    {
      Serial.println("off");
      digitalWrite(relay1, LOW);
    }
    // if message is an Int run for that amount of time
    else if (checkStringIsNumerical(messageIn))
    {
      int runTime = messageIn.toInt();
      digitalWrite(relay1, HIGH);
      delay(runTime * 1000);
      digitalWrite(relay1, LOW);
    }
  }

  // if (String(topic) == "juicebox2/relay1")
  // {
  //   Serial.print("Changing Relay 1 output to ");
  //   if (messageIn == "on")
  //   {
  //     Serial.println("on");
  //     digitalWrite(relay1, HIGH);
  //   }
  //   else if (messageIn == "off")
  //   {
  //     Serial.println("off");
  //     digitalWrite(relay1, LOW);
  //   }
  // }
  // if (String(topic) == "juicebox2/relay2")
  // {
  //   Serial.print("Changing Relay 2 output to ");
  //   if (messageIn == "on")
  //   {
  //     Serial.println("on");
  //     digitalWrite(relay2, HIGH);
  //   }
  //   else if (messageIn == "off")
  //   {
  //     Serial.println("off");
  //     digitalWrite(relay2, LOW);
  //   }
  // }
  // if (String(topic) == "juicebox2/relay3")
  // {
  //   Serial.print("Changing Relay 3 output to ");
  //   if (messageIn == "on")
  //   {
  //     Serial.println("on");
  //     digitalWrite(relay3, HIGH);
  //   }
  //   else if (messageIn == "off")
  //   {
  //     Serial.println("off");
  //     digitalWrite(relay3, LOW);
  //   }
  // }
  // if (String(topic) == "juicebox2/relay6")
  // {
  //   Serial.print("Changing Relay 6 output to ");
  //   if (messageIn == "on")
  //   {
  //     Serial.println("on");
  //     digitalWrite(relay6, HIGH);
  //   }
  //   else if (messageIn == "off")
  //   {
  //     Serial.println("off");
  //     digitalWrite(relay6, LOW);
  //   }
  // }
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
      mqttClient.publish("juicebox2", "hello world");
      // ... and resubscribe
      mqttClient.subscribe("juicebox2/#");
      // mqttClient.subscribe("juicebox2/relay2");
      // mqttClient.subscribe("juicebox2/relay3");
      // mqttClient.subscribe("juicebox1/relay6");
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

void setup()
{
  // pin setup
  // relays
  pinMode(relay1, OUTPUT);
  // digitalWrite(relay1, HIGH); // !start relay1 HIGH (Which is off????)
  pinMode(relay2, OUTPUT);
  // digitalWrite(relay2, HIGH); // !start relay1 HIGH (Which is off????)
  pinMode(relay3, OUTPUT);
  pinMode(relay6, OUTPUT);
  // digitalWrite(relay3, HIGH); // !start relay1 HIGH (Which is off????)
  // leds
  // pinMode(greenLed, OUTPUT);
  // pinMode(blueLed, OUTPUT);
  Serial.begin(9600);

  connectToWiFi();
  // setup MQTT client
  mqttClient.setServer(MQTTSERVER, 1883);
  mqttClient.setCallback(callback);

  // OTA Setup
  ArduinoOTA.setHostname("juicebox_2");
  ArduinoOTA.onStart([]()
                     { Serial.println("Start"); });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
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
  // printScales();
  ArduinoOTA.handle();
  mqttClient.loop();
  // not using - will try again later!
}