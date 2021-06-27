#include "secrets.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char *SSID = SECRET_SSID;
const char *PWD = SECRET_PWD;
// Led Pins
const int statusLed = 2;
const int greenLed = 12;
const int blueLed = 14;
// Relay Pins
const int relay1 = 27;
const int relay2 = 26;

// JSON data buffer
StaticJsonDocument<250> jsonDocument;
char buffer[250];

// Web server running on port 80
WebServer server(80);
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

void handleLedOn()
{
  if (server.hasArg("plain") == false)
  {
    //handle error here
  }
  String body = server.arg("plain");
  deserializeJson(jsonDocument, body);
  int time = jsonDocument["time"];
  String timeString = String(time);
  String messageToSend = String("{\"message\": \"running greeen blue for: " + timeString + "ms\" }");
  if (jsonDocument["relay"] == 1)
  {
    digitalWrite(relay1, LOW);    // turn on relay 1
    digitalWrite(greenLed, HIGH); // turn on the LED
    delay(time);                  // wait for half a second or 500 milliseconds
    digitalWrite(relay1, HIGH);   // turn off relay 1
    digitalWrite(greenLed, LOW);  // turn off the LED
  }
  if (jsonDocument["relay"] == 2)
  {
    digitalWrite(relay2, LOW);    // turn on relay 1
    digitalWrite(greenLed, HIGH); // turn on the LED
    delay(time);                  // wait for half a second or 500 milliseconds
    digitalWrite(relay2, HIGH);   // turn off relay 1
    digitalWrite(greenLed, LOW);  // turn off the LED
  }

  // server.send(200, "application/json", messageToSend);
  // digitalWrite(relay1, LOW);    // turn on relay 1
  // digitalWrite(greenLed, HIGH); // turn on the LED
  // delay(time);                  // wait for half a second or 500 milliseconds
  // digitalWrite(relay1, HIGH);   // turn off relay 1
  // digitalWrite(greenLed, LOW);  // turn off the LED
  // digitalWrite(blueLed, HIGH);  // turn off the LED
  // digitalWrite(relay2, LOW);    // turn off relay 1

  // delay(time);
  // digitalWrite(blueLed, LOW); // turn on the LED
  // digitalWrite(relay2, HIGH); // turn off relay 1
}

void setup_routing()
{
  server.on("/ledon", HTTP_POST, handleLedOn);

  // start server
  server.begin();
}

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

  // Sensor setup
  // if (!bme.begin(0x76)) {
  // Serial.println("Problem connecting to BME280");
  // }

  connectToWiFi();

  // setup_task();
  setup_routing();
  // Initialize Neopixel
  // pixels.begin();
}

void loop()
{
  server.handleClient();
  // led test
  // wait for half a second or 500 milliseconds
}