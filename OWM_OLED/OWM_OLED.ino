/*********************************************************************
  Thibault MARECHAL
  02/12/2018
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WifiClientSecure.h>
#include <ArduinoJson.h>

#include "display.h"
#include "timeConversion.h"
#include "weatherMap.h"
#include "wifiSettings.h"

#define DELAY 10000
String line1, line2, line3, line4;
String reception;

void setup() {

  // set I2C pins (2 for SDA and 0 for SCL)
  Wire.begin(2, 0);

  setupDisplay();

  //Connect to Wifi & display
  WiFi.begin(wifi_ssid, wifi_password);
  display.print("Connecting to Wifi");

  while (WiFi.status() != WL_CONNECTED)
  {
    display.print(".");
    display.display();
    delay(1000);
  }

  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Connected, IP address: \n");
  display.print(WiFi.localIP());
  display.display();

}


void loop() {

  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  //test connection
  if (!client.connect(host, httpsPort)) {
    display.print("Couldn't connect to the API.");
    delay(DELAY);
    return;
  }
  //Send a request
  client.print(String("GET ") + param + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n\r\n");
  reception = client.readStringUntil('\n');
  if (reception != "HTTP/1.1 200 OK\r") {
    display.print("Unexpected answer...");
    delay(DELAY);
    return;
  }
  //Received a valid answer
  if (client.find("\r\n\r\n")) {
    DynamicJsonBuffer jsonBuffer(4096);
    JsonObject& root = jsonBuffer.parseObject(client);

    const char* time = root["dt"];
    line1 = getHumanTime(time);    line1 += "\n";

    const double temp = root["main"]["temp"];
    line2 = "Temp : ";    line2 += temp;    line2 += " C\n";

    const double wind = root["wind"]["speed"];
    line3 = "Wind : ";    line3 += wind;    line3 += "m/s\n";

    const char* weather = root["weather"][0]["main"];
    line4 = "Weather: ";    line4 += weather;

    //Write on oled
    //Available data for ["weather"][0]["main"] -> "Clear", "Clouds", "Rain" or "Snow"
    //Todo draw a Symbol for cloud, clear sky, or rain/snow.
    displayAllLines(line1, line2, line3, line4);
  }
  digitalWrite(2, HIGH);
  delay(500);
  digitalWrite(2, LOW);
  delay(DELAY);
  client.stop();
}
