#include "secrets.h"
#include <WiFi.h>
#include <WebServer.h>
const char *SSID = SECRET_SSID;
const char *PWD = SECRET_PWD;
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

const int greenLed = 2;

void setup()
{
  pinMode(greenLed, OUTPUT);
  Serial.begin(9600);

  // Sensor setup
  // if (!bme.begin(0x76)) {
  // Serial.println("Problem connecting to BME280");
  // }

  connectToWiFi();

  // setup_task();
  // setup_routing();
  // Initialize Neopixel
  // pixels.begin();
}

void loop()
{
  // server.handleClient();
  // led test
  digitalWrite(greenLed, HIGH); // turn on the LED
  delay(500);                   // wait for half a second or 500 milliseconds
  digitalWrite(greenLed, LOW);  // turn off the LED
  delay(500);                   // wait for half a second or 500 milliseconds
}