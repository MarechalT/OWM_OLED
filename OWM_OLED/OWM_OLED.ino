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

char line1[64], line2[64], line3[64], line4[64];
String reception;

void setup(){
  
  //Serial used for Debugging purpose
  Serial.begin(115200);
  Serial.println("Starting setup");

  // set I2C pins (2 for SDA and 0 for SCL)
  Wire.begin(2, 0);

  setupDisplay();
  
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

  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  Serial.print("requesting URL: ");
  Serial.println(openWeatherMapUrl);

  client.print(String("GET ") + param + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: ESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  reception = client.readStringUntil('\n');
  if (reception != "HTTP/1.1 200 OK\r") {
    Serial.print("Unexpected response: ");
    Serial.println(reception);
    return;
  }
  if (client.find("\r\n\r\n")) {
    DynamicJsonBuffer jsonBuffer(4096);
    JsonObject& root = jsonBuffer.parseObject(client);
    //root.prettyPrintTo(Serial);

    // parsed output
    const char* name = root["name"];
    Serial.print("City: "); Serial.println(name);

    const char* time = root["dt"];
    Serial.print("time: "); Serial.println(time);
    //sprintf(line1,"%s\n",getHumanTime(time));
    strcat(line1,getHumanTime(time));
    strcat(line1,"\n");
    Serial.println(line1);
    
    const double temp = root["main"]["temp"];
    sprintf(line2, "Temp : %.02f C\n", temp);
    Serial.println(line2);


    const double wind = root["wind"]["speed"];
    sprintf(line3, "Wind : %.02f m/s\n", wind);
    Serial.println(line3);

    const char* weather = root["weather"][0]["main"];
    const char* description = root["weather"][0]["description"];
    sprintf(line4, "Weather: %s \n (%s)", weather, description);
    Serial.println(line4);

    //Write on oled
    //Available data for ["weather"][0]["main"] -> "Clear", "Clouds", "Rain" or "Snow"


    display.clearDisplay();
    display.setCursor(0, 0);
    display.print(line1);
    display.print(line2);
    display.print(line3);
    display.print(line4);
    display.display();

  }
}


void loop() {

}
