
#ifndef WEATHERMAP_H_
#define WEATHERMAP_H_

const char* host = "api.openweathermap.org";
const int   httpsPort = 443;
const char* openWeatherMapUrl = "/data/2.5/forecast";
const char* openWeatherMapId = "3081368"; // Wroclaw, PL
const char* openWeatherMapQ = "Wroclaw"; //
const char* openWeatherMapUnits = "metric"; // Celsius
const char* openWeatherKey = "a2ab272b15cbf3105ea81634d405bf2f";
String param = String(openWeatherMapUrl) + "?q=" + openWeatherMapQ + "&cnt=2&units=" + openWeatherMapUnits + "&APPID=" + openWeatherKey;

#endif
