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
#define imageDelay 3000
#define errorDelay 1000
String reception;
const unsigned int tryMax = 10;

void setup() {
  // set I2C pins (2 for SDA and 0 for SCL)
  Wire.begin(2, 0);
  //setup the OLED
  setupDisplay();
  WiFi.mode(WIFI_STA);
//  wifi_set_sleep_type(LIGHT_SLEEP_T);
}

bool connectWifi() {
  WiFi.begin(wifi_ssid, wifi_password);
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Connecting to Wifi");
  unsigned int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < tryMax)
  {
    tries++;
    display.print(".");
    display.display();
    delay(errorDelay);
  }
  return (tries < tryMax) ? true : false;
}

void printWifiAddress() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Connected, IP address: \n");
  display.print(WiFi.localIP());
  display.display();
}

void disconnectWifi() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Stopping all connections \n");
  WiFi.disconnect(true);
}

void displayValues(const JsonObject &root) {
  String line1, line2, line3, line4;
  line1 = "\n";
  const double temp = root["list"][1]["main"]["temp"];
  line2 = "Temp : ";    line2 += temp;    line2 += " C\n";
  const double wind = root["list"][1]["wind"]["speed"];
  line3 = "Wind : ";    line3 += wind;    line3 += "m/s\n";
  const char* weather = root["list"][1]["weather"][0]["main"];
  line4 = "Weather: ";    line4 += weather;

  int type;
  if (!strcmp(weather, "Clear"))
    type = weatherType::sun;
  else if (!strcmp(weather, "Clouds"))
    type = weatherType::cloud;
  else if (!strcmp(weather, "Rain"))
    type = weatherType::rain;
  else if (!strcmp(weather, "Snow"))
    type = weatherType::snow;
  else
    type = weatherType::Max;

  //Write on oled
  int i = 0;
  while (i < 360) {
    displayWeatherType(type);
    
    delay(imageDelay);
    displayWeather1(line1, line2, line3, line4);
    delay(imageDelay);
    i++;
  }
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    if (!connectWifi()) {
      display.print("Couldn't connect to the WiFi network.");
      delay(errorDelay);
      disconnectWifi();
    }
  }

  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  //test connection
  if (!client.connect(host, httpsPort)) {
    display.print("Couldn't connect to the API.");
    delay(errorDelay);
    client.stop();
    disconnectWifi();
    return;
  }

  //Send a request
  client.print(String("GET ") + param + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "User-Agent: ESP8266\r\n" + "Connection: close\r\n\r\n");
  reception = client.readStringUntil('\n');
  if (reception != "HTTP/1.1 200 OK\r") {
    display.print("Unexpected answer...");
    delay(errorDelay);
    client.stop();
    disconnectWifi();
    return;
  }

  //Received a valid answer
  if (client.find("\r\n\r\n")) {
    DynamicJsonBuffer jsonBuffer(4096);
    JsonObject& root = jsonBuffer.parseObject(client);
    client.stop();
    disconnectWifi();
    displayValues(root);
  }
}
