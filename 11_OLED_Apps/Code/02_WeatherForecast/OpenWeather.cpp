/*	OpenWeatherMap.cpp
	Jim Lindblom <jim@sparkfun.com>
	August 31, 2015
	
	This is a simple library for interacting with the
	OpenWeatherMap API:
	http://openweathermap.org/api
	
	Development environment specifics:
	Particle Build environment (https://www.particle.io/build)
	Particle Photon
	
	Distributed as-is; no warranty is given. 
*/

#include "OpenWeatherMap.h"

String weatherServer = "api.openweathermap.org";

OpenWeather::OpenWeather()
{
  _apiKey = NULL;
  _units = IMPERIAL;
}

OpenWeather::OpenWeather(String apiKey)
{
  _apiKey = apiKey;
}

void OpenWeather::setUnits(t_forecast_units units)
{
  _units = units;
}

int OpenWeather::current(float lat, float lon)
{
  return currentForecast("lat=" + String(lat) + "&lon=" + String(lon));
}

int OpenWeather::current(String cityName)
{
  return currentForecast("q=" + cityName);
}

int OpenWeather::current(uint32_t cityID)
{
  return currentForecast("id=" + String(cityID));
}

int OpenWeather::hourly(float lat, float lon, unsigned int tripleHours)
{
  return hourlyForecast("lat=" + String(lat) + "&lon=" + String(lon), tripleHours);
}

int OpenWeather::hourly(String cityName, unsigned int tripleHours)
{
  return hourlyForecast("q=" + cityName, tripleHours);
}

int OpenWeather::hourly(uint32_t cityID, unsigned int tripleHours)
{
  return hourlyForecast("id=" + String(cityID), tripleHours);
}

int OpenWeather::daily(float lat, float lon, unsigned int days)
{
  if (days > 10)
    return -1;
  return dailyForecast("lat=" + String(lat) + "&lon=" + String(lon), days);
}

float OpenWeather::temperature()
{
  return forecast.temperature.value.toFloat();
}

float OpenWeather::maxTemperature()
{
  return forecast.temperature.maxT.toFloat();
}

float OpenWeather::minTemperature()
{
  return forecast.temperature.minT.toFloat();
}

unsigned int OpenWeather::humidity()
{
  return forecast.humidity.value.toInt();
}

float OpenWeather::pressure()
{
  return forecast.pressure.value.toFloat();
}

String OpenWeather::getDate()
{
  int timeStart = forecast.day.indexOf('T');

  return forecast.day.substring(0, timeStart);
}

String OpenWeather::getTime()
{
  if (forecast.type == DAILY_FORECAST)
    return NULL;

  int timeStart = forecast.day.indexOf('T');
  return forecast.day.substring(timeStart + 1);
}

float OpenWeather::windSpeed()
{
  return forecast.wind.speedValue.toFloat();
}

String OpenWeather::windDirection()
{
  return forecast.wind.dirCode;
}

String OpenWeather::windName()
{
  return forecast.wind.speedName;
}

String OpenWeather::precipitationType()
{
  if ((forecast.precip.type == "no") || (forecast.precip.type == NULL))
  {
    return NULL;
  }
  return forecast.precip.type;
}

float OpenWeather::precipitationValue()
{
  return forecast.precip.value.toFloat();
}

int OpenWeather::conditionID()
{
  return forecast.symbol.number;
}

String OpenWeather::conditionName()
{
  return forecast.symbol.name;
}

int OpenWeather::currentForecast(String location)
{
  /////////////////////////////////////////
  // Connect to client and send HTTP GET //
  /////////////////////////////////////////
  TCPClient client;
  if (client.connect(weatherServer, 80))
  {
      client.print("GET /data/2.5/weather?");
      client.print(location);
      client.print("&mode=xml");
      if (_apiKey != NULL)
        client.print("&APPID=" + _apiKey);
      if (_units == IMPERIAL)
        client.print("&units=imperial"); // Change imperial to metric for celsius (delete this line for kelvin)
      else if (_units == METRIC)
        client.print("&units=metric");
      client.println(" HTTP/1.0");
      client.println("Host: " + weatherServer);
      client.println("Content-length: 0");
      client.println();
  }
  else
  {
      return 0;
  }

  ///////////////////////////////////////
  // Wait for Response, Store in Array //
  ///////////////////////////////////////
  String rsp;
  int timeout = 1000;
  while ((!client.available()) && (timeout-- > 0))
    delay(1);
  if (timeout <= 0)
    return 0;
  while (client.available())
  {
    char c = client.read();
    rsp += c;
  }

  ////////////////
  // Disconnect //
  ////////////////
  client.stop();
  Serial.println("Response: ");
  Serial.println(rsp);

  //////////////////////////////
  // Sort Data into Variables //
  //////////////////////////////
  forecast.temperature.value = "" + parseXML(&rsp, "temperature", "value");
  forecast.temperature.minT = "" + parseXML(&rsp, "temperature", "min");
  forecast.temperature.maxT = "" + parseXML(&rsp, "temperature", "max");
  forecast.temperature.unit = "" + parseXML(&rsp, "temperature", "unit");
  forecast.humidity.value = "" + parseXML(&rsp, "humidity", "value");
  forecast.humidity.unit = "" + parseXML(&rsp, "humidity", "unit");
  forecast.pressure.value = "" + parseXML(&rsp, "pressure", "value");
  forecast.pressure.unit = "" + parseXML(&rsp, "pressure", "unit");
  forecast.wind.speedValue = "" + parseXML(&rsp, "speed", "value");
  forecast.wind.speedName = "" + parseXML(&rsp, "speed", "name");
  forecast.wind.dirDeg = "" + parseXML(&rsp, "direction", "value");
  forecast.wind.dirCode = "" + parseXML(&rsp, "direction", "code");
  forecast.wind.dirName = "" + parseXML(&rsp, "direction", "name");
  forecast.clouds.all = "" + parseXML(&rsp, "clouds", "value");
  forecast.clouds.value = "" + parseXML(&rsp, "clouds", "name");
  forecast.vis.value = "" + parseXML(&rsp, "visibility", "value");
  forecast.precip.value = "" + parseXML(&rsp, "precipitation", "value");
  forecast.precip.type = "" + parseXML(&rsp, "precipitation", "mode");
  forecast.symbol.number = parseXML(&rsp, "weather", "number").toInt();
  forecast.symbol.name = "" + parseXML(&rsp, "weather", "value");
  forecast.day = "" + parseXML(&rsp, "lastupdate", "value");
  forecast.type = CURRENT_FORECAST;

  //printDebug();

  return 1;
}

int OpenWeather::hourlyForecast(String location, unsigned int count)
{
  /////////////////////////////////////////
  // Connect to client and send HTTP GET //
  /////////////////////////////////////////
  TCPClient client;
  if (client.connect(weatherServer, 80))
  {
      client.print("GET /data/2.5/forecast?");
      client.print(location);
      client.print("&cnt=" + String(count));
      client.print("&mode=xml");
      if (_apiKey != NULL)
        client.print("&APPID=" + _apiKey);
      if (_units == IMPERIAL)
        client.print("&units=imperial"); // Change imperial to metric for celsius (delete this line for kelvin)
      else if (_units == METRIC)
        client.print("&units=metric");
      client.println(" HTTP/1.0");
      client.println("Host: " + weatherServer);
      client.println("Content-length: 0");
      client.println();
  }
  else
  {
      return 0;
  }

  ///////////////////////////////////////
  // Wait for Response, Store in Array //
  ///////////////////////////////////////
  String rsp;
  unsigned int forecastCount = 0;
  int timeout = 1000;
  while ((!client.available()) && (timeout-- > 0))
    delay(1);
  if (timeout <= 0)
    return 0;
  while (client.available())
  {
    char c = client.read();
    rsp += c;
    if (rsp.indexOf("</time>") > 0)
    {
      forecastCount++;
      if (forecastCount < count)
      {
        rsp = NULL; // Clear out response
      }
      else
      {
        break;
      }
    }
  }

  ////////////////
  // Disconnect //
  ////////////////
  client.stop();
  Serial.println("Response: ");
  Serial.println(rsp);

  forecast.day = "" + parseXML(&rsp, "time", "from");
  forecast.symbol.number = parseXML(&rsp, "symbol", "number").toInt();
  forecast.symbol.name = "" + parseXML(&rsp, "symbol", "name");
  forecast.precip.value = "" + parseXML(&rsp, "precipitation", "value");
  forecast.precip.type = "" + parseXML(&rsp, "precipitation", "type");
  forecast.wind.dirDeg = "" + parseXML(&rsp, "windDirection", "deg");
  forecast.wind.dirCode = "" + parseXML(&rsp, "windDirection", "code");
  forecast.wind.dirName = "" + parseXML(&rsp, "windDirection", "name");
  forecast.wind.speedValue = "" + parseXML(&rsp, "windSpeed", "mps");
  forecast.wind.speedName = "" + parseXML(&rsp, "windSpeed", "name");
  forecast.temperature.value = "" + parseXML(&rsp, "temperature", "value");
  forecast.temperature.minT = "" + parseXML(&rsp, "temperature", "min");
  forecast.temperature.maxT = "" + parseXML(&rsp, "temperature", "max");
  forecast.temperature.nightT = "" + parseXML(&rsp, "temperature", "night");
  forecast.temperature.eveT = "" + parseXML(&rsp, "temperature", "eve");
  forecast.temperature.mornT = "" + parseXML(&rsp, "temperature", "morn");
  forecast.pressure.unit = "" + parseXML(&rsp, "pressure", "unit");
  forecast.pressure.value = "" + parseXML(&rsp, "pressure", "value");
  forecast.humidity.value = "" + parseXML(&rsp, "humidity", "value");
  forecast.humidity.unit = "" + parseXML(&rsp, "humidity", "unit");
  forecast.clouds.value = "" + parseXML(&rsp, "clouds", "value");
  forecast.clouds.all = "" + parseXML(&rsp, "clouds", "all");
  forecast.clouds.unit = "" + parseXML(&rsp, "clouds", "unit");
  forecast.type = HOURLY_FORECAST;

  //printDebug();

  return 1;
}

int OpenWeather::dailyForecast(String location, unsigned int count)
{
  /////////////////////////////////////////
  // Connect to client and send HTTP GET //
  /////////////////////////////////////////
  TCPClient client;
  if (client.connect(weatherServer, 80))
  {
      client.print("GET /data/2.5/forecast/daily?");
      client.print(location);
      client.print("&cnt=" + String(count));
      client.print("&mode=xml");
      if (_apiKey != NULL)
        client.print("&APPID=" + _apiKey);
      if (_units == IMPERIAL)
        client.print("&units=imperial"); // Change imperial to metric for celsius (delete this line for kelvin)
      else if (_units == METRIC)
        client.print("&units=metric");
      client.println(" HTTP/1.0");
      client.println("Host: " + weatherServer);
      client.println("Content-length: 0");
      client.println();
  }
  else
  {
      return 0;
  }

  ///////////////////////////////////////
  // Wait for Response, Store in Array //
  ///////////////////////////////////////
  String rsp;
  unsigned int forecastCount = 0;
  int timeout = 1000;
  while ((!client.available()) && (timeout-- > 0))
    delay(1);
  if (timeout <= 0)
    return 0;
  while (client.available())
  {
    char c = client.read();
    rsp += c;
    if (rsp.indexOf("</time>") > 0)
    {
      forecastCount++;
      if (forecastCount < count)
      {
        rsp = NULL; // Clear out response
      }
      else
      {
        break;
      }
    }
  }

  ////////////////
  // Disconnect //
  ////////////////
  client.stop();
  Serial.println("Response: ");
  Serial.println(rsp);

  forecast.day = "" + parseXML(&rsp, "time", "day");
  forecast.symbol.number = parseXML(&rsp, "symbol", "number").toInt();
  forecast.symbol.name = "" + parseXML(&rsp, "symbol", "name");
  forecast.precip.value = "" + parseXML(&rsp, "precipitation", "value");
  forecast.precip.type = "" + parseXML(&rsp, "precipitation", "type");
  forecast.wind.dirDeg = "" + parseXML(&rsp, "windDirection", "deg");
  forecast.wind.dirCode = "" + parseXML(&rsp, "windDirection", "code");
  forecast.wind.dirName = "" + parseXML(&rsp, "windDirection", "name");
  forecast.wind.speedValue = "" + parseXML(&rsp, "windSpeed", "mps");
  forecast.wind.speedName = "" + parseXML(&rsp, "windSpeed", "name");
  forecast.temperature.value = "" + parseXML(&rsp, "temperature", "day");
  forecast.temperature.minT = "" + parseXML(&rsp, "temperature", "min");
  forecast.temperature.maxT = "" + parseXML(&rsp, "temperature", "max");
  forecast.temperature.nightT = "" + parseXML(&rsp, "temperature", "night");
  forecast.temperature.eveT = "" + parseXML(&rsp, "temperature", "eve");
  forecast.temperature.mornT = "" + parseXML(&rsp, "temperature", "morn");
  forecast.pressure.unit = "" + parseXML(&rsp, "pressure", "unit");
  forecast.pressure.value = "" + parseXML(&rsp, "pressure", "value");
  forecast.humidity.value = "" + parseXML(&rsp, "humidity", "value");
  forecast.humidity.unit = "" + parseXML(&rsp, "humidity", "unit");
  forecast.clouds.value = "" + parseXML(&rsp, "clouds", "value");
  forecast.clouds.all = "" + parseXML(&rsp, "clouds", "all");
  forecast.clouds.unit = "" + parseXML(&rsp, "clouds", "unit");
  forecast.type = DAILY_FORECAST;

  //printDebug();

  return 1;
}

String OpenWeather::parseXML(String * search, String tag, String attribute)
{
    int tagStart = tagIndex(search, tag, 1);
    int tagEnd = tagIndex(search, tag, 0);
    if (tagStart >= 0)
    {
      int attributeStart = search->indexOf(attribute, tagStart);
      if ((attributeStart >= 0) && (attributeStart < tagEnd)) // Make sure we don't get value of another key
      {
        attributeStart = search->indexOf("\"", attributeStart);
        if (attributeStart >= 0)
        {
            int attributeEnd = search->indexOf("\"", attributeStart + 1);
            if (attributeEnd >= 0)
            {
                return search->substring(attributeStart + 1, attributeEnd);
            }
        }
      }
    }

    return NULL;
}

int OpenWeather::tagIndex(String * xml, String tag, bool start)
{
  String fullTag = "<";
  if (start)
  {
    fullTag += tag;
    fullTag += ' '; // Look for a space after the tag name
  }
  else
  {
    fullTag += '/';
    fullTag += tag;
    fullTag += '>';
  }

  return xml->indexOf(fullTag);
}

void OpenWeather::printDebug()
{
  Serial.println("=====================================================");
  Serial.print("day: "); Serial.println(forecast.day);
  Serial.print("symbol.number: "); Serial.println(forecast.symbol.number);
  Serial.print("symbol.name: "); Serial.println(forecast.symbol.name);
  Serial.print("precip.value: "); Serial.println(forecast.precip.value);
  Serial.print("precip.type: "); Serial.println(forecast.precip.type);
  Serial.print("wind.dirDeg: "); Serial.println(forecast.wind.dirDeg);
  Serial.print("wind.dirCode: "); Serial.println(forecast.wind.dirCode);
  Serial.print("wind.dirName: "); Serial.println(forecast.wind.dirName);
  Serial.print("wind.speedValue: "); Serial.println(forecast.wind.speedValue);
  Serial.print("wind.speedName: "); Serial.println(forecast.wind.speedName);
  Serial.print("temperature.value: "); Serial.println(forecast.temperature.value);
  Serial.print("temperature.minT: "); Serial.println(forecast.temperature.minT);
  Serial.print("temperature.maxT: "); Serial.println(forecast.temperature.maxT);
  Serial.print("temperature.unit: "); Serial.println(forecast.temperature.unit);
  Serial.print("pressure.unit: "); Serial.println(forecast.pressure.unit);
  Serial.print("pressure.value: "); Serial.println(forecast.pressure.value);
  Serial.print("humidity.value: "); Serial.println(forecast.humidity.value);
  Serial.print("humidity.unit: "); Serial.println(forecast.humidity.unit);
  Serial.print("clouds.value: "); Serial.println(forecast.clouds.value);
  Serial.print("clouds.all: "); Serial.println(forecast.clouds.all);
  Serial.print("vis.value: "); Serial.println(forecast.vis.value);
  Serial.println("=====================================================");
}
