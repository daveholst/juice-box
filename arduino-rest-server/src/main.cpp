#include "secrets.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
// #include <WebSocketsClient.h>
#include <ArduinoWebsockets.h>

const char *SSID = SECRET_SSID;
const char *PWD = SECRET_PWD;
// Led Pins
const int statusLed = 2;
const int greenLed = 12;
const int blueLed = 14;
// Relay Pins
const int relay1 = 27;
const int relay2 = 26;

// Allocate the JSON document
StaticJsonDocument<200> doc;

const char *websockets_server_host = "192.168.10.98"; //Enter server adress
const uint16_t websockets_server_port = 3000;         // Enter server port
using namespace websockets;

// Web Sockets client
WebsocketsClient client;

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

// void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
// {
//   // const String message = doc["testMessage"];
//   const String message = doc["testMessage"];
//   Serial.print(message);
// }

void setup()
{
  // pin setup
  // relays
  pinMode(relay1, OUTPUT);
  digitalWrite(relay1, HIGH); // !start relat1 HIGH (Which is off????)
  pinMode(relay2, OUTPUT);
  digitalWrite(relay2, HIGH); // !start relat1 HIGH (Which is off????)

  //leds
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  Serial.begin(9600);

  connectToWiFi();

  // server address, port, and URL path
  // client.begin("192.168.10.98", 3000, "/");

  // event handler
  // client.onEvent(webSocketEvent);

  // try ever 5000 again if connection has failed
  // client.setReconnectInterval(5000);
  Serial.println("Connected to Wifi, Connecting to server.");
  // try to connect to Websockets server
  bool connected = client.connect(websockets_server_host, websockets_server_port, "/");
  if (connected)
  {
    Serial.println("Connected!");
    client.send("Hello Server");
  }
  else
  {
    Serial.println("Not Connected!");
  }

  // run callback when messages are received
  client.onMessage([&](WebsocketsMessage message)
                   {
                     String body = message.data();
                     // Deserialize the JSON document
                     DeserializationError error = deserializeJson(doc, body);

                     // Test if parsing succeeds.
                     if (error)
                     {
                       Serial.print(F("deserializeJson() failed: "));
                       Serial.println(error.f_str());
                       return;
                     }
                     if (doc["device"] == "relay-1" && doc["action"] == "turn_on")
                     {
                       digitalWrite(relay1, LOW);
                     }
                     if (doc["device"] == "relay-1" && doc["action"] == "turn_off")
                     {
                       digitalWrite(relay1, HIGH);
                     }

                     const char *device = doc["device"];
                     Serial.print("Got Message: ");
                     Serial.println(device);
                   });
}

void loop()
{
  // let the websockets client check for incoming messages
  if (client.available())
  {
    client.poll();
  }
  delay(500);
}