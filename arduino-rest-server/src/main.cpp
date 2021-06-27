#include "secrets.h"
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char *SSID = SECRET_SSID;
const char *PWD = SECRET_PWD;
const int statusLed = 2;
const int greenLed = 12;
const int blueLed = 14;
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

  server.send(200, "application/json", messageToSend);
  digitalWrite(greenLed, HIGH); // turn on the LED
  delay(time);                  // wait for half a second or 500 milliseconds
  digitalWrite(greenLed, LOW);  // turn off the LED
  digitalWrite(blueLed, HIGH);  // turn off the LED
  delay(time);
  digitalWrite(blueLed, LOW); // turn on the LED
}
void setup_routing()
{
  server.on("/ledon", HTTP_POST, handleLedOn);

  // start server
  server.begin();
}

void setup()
{
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