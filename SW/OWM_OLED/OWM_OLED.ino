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
#define imageDelay 5000
#define errorDelay 1000
String reception;
const unsigned int tryMax = 10;

void setup() {
  Serial.begin(115200); //Debug only
  Serial.println("Starting program...");
  // set I2C pins (2 for SDA and 0 for SCL)
  Wire.begin(2, 0);
  //setup the OLED
  setupDisplay();
}

bool connectWifi() {
  Serial.println("Connecting to Wifi.");
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
  Serial.println("Disconnecting the Wifi.");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Stopping all connections \n");
  WiFi.disconnect(true);
}

void displayValues(const JsonObject &root) {
  String line1, line2, line3, line4;
  line1 = "\n";
  const double temp = root["main"]["temp"];
  line2 = "Temp : ";    line2 += temp;    line2 += " C\n";
  const double wind = root["wind"]["speed"];
  line3 = "Wind : ";    line3 += wind;    line3 += "m/s\n";
  const char* weather = root["weather"][0]["main"];
  line4 = "Weather: ";    line4 += weather;

  int type;
  Serial.println(weather);
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
  Serial.println("Connected to WiFi.");

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
  Serial.println("Connected to API.");

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
    Serial.println("Received Data from API.");

  //Received a valid answer
  if (client.find("\r\n\r\n")) {
    DynamicJsonBuffer jsonBuffer(4096);
    JsonObject& root = jsonBuffer.parseObject(client);
    client.stop();
    disconnectWifi();
    displayValues(root);
  }
}
