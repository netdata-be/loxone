// PicoC code here
// BB20130727
// Loxone Wunderground weather service script
//
// This program calls the wunderground.com weather web service.
// The return value is a XML document. Since PicoC does not support
// XML so the string needs to be scraped to find the values.

// initially adapted from Paul Sinnema's website "Poor man's weather station" 
// at http://sinnema.ch/?cat=11
//
// Now using the www.wunderground.com website leveraging Jasper's code on:
// http://forum.loxone.com/enen/faqs-tutorials-and-howto-s/2131-creating-your-own-weather-service-when-loxone-s-weather-service-is-not-available-6.html> 


#define BUFF_SIZE 80000
#define RD_BLOCK_SIZE 128
#define REFRESH_MIN 15 // refresh every 15 minutes (developer account 500 calls/day -->max 20 calls per hour)
#define DEBUG_LEVEL 0  // 0 = disable ; 1 = INFO ; 2 = DEBUG

// Settings
// create yourself a (free) developer account on wunderground website; it can handle up to 500 calls/day for free.
char* apiKey   = "YOUR-API-KEY";         // your personal 8 hex characters api key from your account at Wunderground (need to request this key once !)
char* location = "YOUR-LOCATION";        // your location
char* host     = "api.wunderground.com"; // The API URL in orde to get this working you need to use a DNS server


// Helper methods
char* position;
char* downloadData()
{
  char device[255];
  sprintf(device, "/dev/tcp/%s/80", host);
  char headers[255];
  sprintf(headers, "GET /api/%s/conditions/hourly/q/%s.xml HTTP/1.1\r\nHost: %s\r\nUser-Agent: LoxLIVE [en]\r\nContent-Type: text/html; charset=utf-8\r\n\r\n", apiKey, location, host);
  STREAM* tcpStream = stream_create(device, 0, 0);
  stream_write(tcpStream, headers, strlen(headers));
  stream_flush(tcpStream);
  char buffer[BUFF_SIZE];
  char block[RD_BLOCK_SIZE];
  int count;
  int i = 0;
  // read stream
  do
  {
    count = stream_read(tcpStream, block, RD_BLOCK_SIZE, 4000);
    if (count > 0) strncpy((char*)buffer + i * RD_BLOCK_SIZE, block, count);
    i++;
  }
  while (count > 0);
  stream_close(tcpStream);
  free(device);
  free(headers);
  free(block);
  return (char*)buffer;
}

int moveToKey(char* key)
{
  //printf("moving to %s, current pos %d", key, (int)position);
  char* newPos = strstr(position, key);
  if (newPos != NULL)
  {
    position = newPos;
    //printf("found %s, pos %d", key, (int)position);
    return 1;
  }
  return 0;
}


char* readStringValue(char* key, int stripEnd)
{
  char value[50];
  value[49] = '\0';

  if (moveToKey(key) > 0)
  {
    char* valueStart = (char*)((int)position + strlen(key));
    int valueLen = strfind(valueStart, "</", 0);
    strncpy(value, valueStart, valueLen - stripEnd);
    // hier enablen om alle ontvangen keys te lezen
    // printf("%s = %s", key, value);
  }
  return value;
}

int readIntValue(char* key)
{
  return batoi(readStringValue(key, 0));
}

float readFloatValue(char* key)
{
  return batof(readStringValue(key, 0));
}

float readPercentageValue(char* key)
{
  return batof(readStringValue(key, 1));
}

int parseWeatherCode(char* code)
{
  if (strcmp(code, "Clear") == 0)                   return 2;
  if (strcmp(code, "Scattered Clouds") == 0)        return 3;
  if (strcmp(code, "Partly Cloudy") == 0)           return 3;
  if (strcmp(code, "Mostly Cloudy") == 0)           return 4;
  if (strcmp(code, "Mostly Sunny") == 0)            return 1;
  if (strcmp(code, "Cloudy") == 0)                  return 5;
  if (strcmp(code, "Overcast") == 0)                return 7;
  if (strcmp(code, "Small Hail") == 0)              return 25;
  if (strfind(code, "Heavy Freezing Rain", 0) >= 0) return 15;
  if (strfind(code, "Heavy Rain Showers", 0) >= 0)  return 17;
  if (strfind(code, "Rain Showers", 0) >= 0)        return 16;
  if (strfind(code, "Freezing Rain", 0) >= 0)       return 14;
  if (strfind(code, "Light Rain", 0) >= 0)          return 10;
  if (strfind(code, "Heavy Rain", 0) >= 0)          return 12;
  if (strfind(code, "Heavy Sleet", 0) >= 0)         return 27;
  if (strfind(code, "Light Sleet", 0) >= 0)         return 25;
  if (strfind(code, "Sleet", 0) >= 0)               return 26;
  if (strfind(code, "Heavy Thunderstorm", 0) >= 0)  return 19;
  if (strfind(code, "Thunderstorm", 0) >= 0)        return 18;
  if (strfind(code, "Heavy Snow Showers", 0) >= 0)  return 24;
  if (strfind(code, "Snow Showers", 0) >= 0)        return 23;
  if (strfind(code, "Rain", 0) >= 0)                return 11;
  if (strfind(code, "Heavy Snow", 0) >= 0)          return 22;
  if (strfind(code, "Light Snow", 0) >= 0)          return 20;
  if (strfind(code, "Snow", 0) >= 0)                return 21;
  if (strfind(code, "Drizzle", 0) >= 0)             return 13;
  if (strfind(code, "Fog", 0) >= 0)                 return 6;
  if (strfind(code, "Sunny", 0) >= 0)               return 2;
  //  printf("No weather code mapping for %s", code);
  return -1;
}

void parseHourly(int hour_offset)
{
  if (DEBUG_LEVEL > 0 ) printf("weatherservice [INFO]: Getting forecast for +%d hour",hour_offset);
  char temperature_var[50];
  sprintf(temperature_var, "outsideTemp_offset_hour_%d", hour_offset);
  int timestamp = readIntValue("<epoch>") - 1230768000;
  if (moveToKey("<temp>")) setweatherdata(1, timestamp, readFloatValue("<metric>"));
  if (DEBUG_LEVEL > 0 ) printf("weatherservice [INFO]: timestamp for weather = %d", timestamp);
  if (DEBUG_LEVEL > 1 ) printf("weatherservice [DEBUG]: Forecasted temp = %d", readFloatValue("<metric>"));
  setio(temperature_var, readFloatValue("<metric>"));
  if (moveToKey("<dewpoint>")) setweatherdata(2, timestamp, readFloatValue("<metric>"));
  int weatherCode = parseWeatherCode(readStringValue("<condition>", 0));
  if (weatherCode > 0)
  {
    if (DEBUG_LEVEL > 1 ) printf("INFO: WeatherCode = %d",weatherCode);
    setweatherdata(10, timestamp, weatherCode);
  }
  if (moveToKey("<wspd>")) setweatherdata(4, timestamp, readFloatValue("<metric>"));
  if (moveToKey("<wdir>")) setweatherdata(5, timestamp, readIntValue("<degrees>"));
  setweatherdata(3, timestamp, readIntValue("<humidity>"));
  if (DEBUG_LEVEL > 1 ) printf("weatherservice [DEBUG]: Forecasted humidity = %d", readIntValue("<humidity>"));
  if (moveToKey("<feelslike>")) setweatherdata(26, timestamp, readFloatValue("<metric>"));
  if (moveToKey("<mslp>")) setweatherdata(11, timestamp, readIntValue("<metric>"));
  // clear precip due to lack of data in forecasts
  setweatherdata(9, timestamp, 0.0);
  free(temperature_var);
}

void parseWeather()
{
  // Current observation
  if (moveToKey("<current_observation>") > 0)
  {
    if (DEBUG_LEVEL > 0 ) printf("weatherservice [INFO]: Getting current weather");
    printf("weatherservice [INFO]: Wunderground observation time: %s", readStringValue("<observation_time>",0));
	int observation_epoch = readIntValue("<observation_epoch>");
    int timestamp = observation_epoch - 1230768000;
    if (DEBUG_LEVEL > 0 ) printf("weatherservice [INFO]: timestamp for weather = %d", timestamp);
    int weatherCode = parseWeatherCode(readStringValue("<weather>", 0));
    if (weatherCode > 0) setweatherdata(10, timestamp, weatherCode);
	float temp_c = readFloatValue("<temp_c>");
    setweatherdata(1, timestamp, temp_c);
    setio("currentOutsideTemp", temp_c);
    setweatherdata(3,  timestamp, readPercentageValue("<relative_humidity>"));
    setweatherdata(5,  timestamp, readIntValue("<wind_degrees>"));
    setweatherdata(4,  timestamp, readFloatValue("<wind_kph>"));
    setweatherdata(11, timestamp, readIntValue("<pressure_mb>"));
    setweatherdata(2,  timestamp, readFloatValue("<dewpoint_c>"));
    setweatherdata(26, timestamp, readFloatValue("<feelslike_c>"));
    setweatherdata(9,  timestamp, readFloatValue("<precip_today_metric>"));
    setweatherdata(22, timestamp, getcurrenttime());
    setweatherdata(23, timestamp, timestamp);
    setweatherdata(12, timestamp, observation_epoch);
  }

  // Hourly forecasts
  int i = 0;
  if (moveToKey("<hourly_forecast>") > 0)
  {
    while (moveToKey("<forecast>") > 0)
    {
      i++;
      parseHourly(i);
    }
  }
}

// Main loop
while(TRUE)
{
  char* data = downloadData();
  printf("weatherservice [INFO]: Wunderground xml data length: %d", strlen(data));
  position = data;
  parseWeather();
  free(data);
  sleeps(REFRESH_MIN * 60);
}
