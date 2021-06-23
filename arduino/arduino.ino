#include <WiFi.h>
#include <WebServer.h>
const char *SSID = "HolstNoT";
const char *PWD = "!jYs6odr@R6%Xf";
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
