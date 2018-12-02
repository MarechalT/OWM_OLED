
#ifndef WEATHERMAP_H_
#define WEATHERMAP_H_

const char* host = "api.openweathermap.org";
const int   httpsPort = 443;
const char* openWeatherMapUrl = "/data/2.5/weather";
const char* openWeatherMapId = "3081368"; // Wroclaw, PL
const char* openWeatherMapQ = "Wroclaw"; //
const char* openWeatherMapUnits = "metric"; // Celsius
const char* openWeatherKey = "yourAPI_Key";
String param = String(openWeatherMapUrl) + "?q=" + openWeatherMapQ + "&units=" + openWeatherMapUnits + "&APPID=" + openWeatherKey;

#endif
