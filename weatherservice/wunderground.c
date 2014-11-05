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
// Wouter D'Haeseleer's version, enhanced by Henk Oosterlinck's suggested improvements
// And adding: isolated hourly data parsing & prevention of occasional crash due to incorrect wunderground data

#define BUFF_SIZE 80001
#define RD_BLOCK_SIZE 128
#define REFRESH_MIN 15 // refresh every 15 minutes (developer account 500 calls/day -->max 20 calls per hour)
#define DEBUG_LEVEL 0  // 0 = disable ; 1 = INFO ; 2 = DEBUG

// Settings
// create yourself a (free) developer account on wunderground website; it can handle up to 500 calls/day for free.
char* apiKey   = "YOUR-API-KEY";         // your personal 8 hex characters api key from your account at Wunderground (need to request this key once !)
char* location = "YOUR-LOCATION";        // your location
char* host     = "api.wunderground.com"; // The API URL in orde to get this working you need to use a DNS server


// Global variables
char value[50];
char data[BUFF_SIZE];
char hourly_data[4096];
int  debug_msg_count = 0;

// Helper methods
void downloadData()
{
  char device[255];
  sprintf(device, "/dev/tcp/%s/80", host);
  char headers[255];
  sprintf(headers, "GET /api/%s/conditions/hourly/q/%s.xml HTTP/1.1\r\nHost: %s\r\nUser-Agent: LoxLIVE [en]\r\nContent-Type: text/html; charset=utf-8\r\n\r\n", apiKey, location, host);
  STREAM* tcpStream = stream_create(device, 0, 0);
  stream_write(tcpStream, headers, strlen(headers));
  stream_flush(tcpStream);
  char block[RD_BLOCK_SIZE];
  int count;
  int i = 0;
  // read stream
  do
  {
    count = stream_read(tcpStream, block, RD_BLOCK_SIZE, 4000);
    if (count > 0) strncpy((char*)data + i * RD_BLOCK_SIZE, block, count);
    i++;
    if (i >= ( ( BUFF_SIZE - 1 ) / RD_BLOCK_SIZE )) count = 0; // avoid buffer overflows
  }
  while (count > 0);
  stream_close(tcpStream);
  data[BUFF_SIZE] = 0; //put null character or end of string at the end.
}

int moveToKey(char* key)
{
  if (DEBUG_LEVEL > 1 ) sleep(250); //provide time to output log entries, otherwise log entries get lost ?!
  char* newPos = strstr(position, key);
  if (newPos != NULL)
  {
    position = newPos;
    if (DEBUG_LEVEL > 1 ) printf("%d: found key %s, pos %d", debug_msg_count++, key, (int)position);
    return 1;
  } else {
    if (DEBUG_LEVEL > 1 ) printf("%d: NOT FOUND key %s", debug_msg_count++, key);
      return 0;
  }
}

int getNextHourlyData() {
  if (moveToKey("<forecast>") > 0)
  {
    char* hourDataStart = (char*)((int)position + strlen("<forecast>"));
    int hourDataLen = strfind(hourDataStart, "</forecast>", 0);
    if (hourDataLen > 0) {
      strncpy(hourly_data, hourDataStart, hourDataLen);
      // increase position to prevent retrieving the same <forecast> again and again
      position++;
      return 1;
    } else {
      if (DEBUG_LEVEL > 1 ) printf("%d: DATA SEEMS INCOMPLETE (getNextHourlyData)", debug_msg_count++);
    }
  }
  return 0;
}

char* readStringValue(char* key, int stripEnd)
{
  value[49] = '\0';
  if (moveToKey(key) > 0)
  {
    char* valueStart = (char*)((int)position + strlen(key));
    int valueLen = strfind(valueStart, "</", 0);
    if (valueLen - stripEnd > 48) {
      // Prevent error with occasional long (weird?) value of e.g. 'humidity', which produces the following error:
      // myweather:93:14 can't assign char* from 
      if (DEBUG_LEVEL > 1 ) printf("%d: DATA TOO LARGE (readStringValue): length=%d", debug_msg_count++, valueLen);
    } else {
      if (valueLen - stripEnd > 0) {
        strncpy(value, valueStart, valueLen - stripEnd);
        if (DEBUG_LEVEL > 1 ) printf("%d: found value of %s = %s (l=%d,s=%d)", debug_msg_count++, key, value, valueLen, stripEnd);
      } else {
        if (DEBUG_LEVEL > 1 ) printf("%d: DATA SEEMS INCOMPLETE (readStringValue)", debug_msg_count++);
      }
    }    
  }
  
  if (DEBUG_LEVEL > 1 ) printf("%d: returning value %s", debug_msg_count++, value);
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
  if (DEBUG_LEVEL > 0 ) printf("%d: No weather code mapping for %s", debug_msg_count++, code);
  return -1;
}

void parseHourly(int hour_offset)
{
  if (DEBUG_LEVEL > 0 ) printf("%d: Getting forecast for +%d hour", debug_msg_count++, hour_offset);
  char temperature_var[50];
  
  sprintf(temperature_var, "outsideTemp_offset_hour_%d", hour_offset);
  int timestamp = readIntValue("<epoch>") - 1230768000;
  if (DEBUG_LEVEL > 1 ) printf("%d: timestamp for weather = %d", debug_msg_count++, timestamp);

  if (moveToKey("<temp>")) {
    float temperature_val = readFloatValue("<metric>");
    setweatherdata(1, timestamp, temperature_val);
    if (DEBUG_LEVEL > 1 ) printf("%d: Forecasted temp = %d", debug_msg_count++, temperature_val);
    setio(temperature_var, temperature_val);
  }
  if (moveToKey("<dewpoint>")) setweatherdata(2, timestamp, readFloatValue("<metric>"));
  int weatherCode = parseWeatherCode(readStringValue("<condition>", 0));
  if (weatherCode > 0)
  {
    if (DEBUG_LEVEL > 1 ) printf("%d: WeatherCode = %d", debug_msg_count++,weatherCode);
    setweatherdata(10, timestamp, weatherCode);
  }
  if (moveToKey("<wspd>")) setweatherdata(4, timestamp, readFloatValue("<metric>"));
  if (moveToKey("<wdir>")) setweatherdata(5, timestamp, readIntValue("<degrees>"));
  setweatherdata(3, timestamp, readIntValue("<humidity>"));
  if (moveToKey("<feelslike>")) setweatherdata(26, timestamp, readFloatValue("<metric>"));
  if (moveToKey("<mslp>")) setweatherdata(11, timestamp, readIntValue("<metric>"));
  // clear precip due to lack of data in forecasts
  setweatherdata(9, timestamp, 0.0);
}

void parseWeather()
{
  // Current observation
  if (moveToKey("<current_observation>") > 0)
  {
    if (DEBUG_LEVEL > 0 ) printf("%d: Getting current weather", debug_msg_count++);
	  int observation_epoch = readIntValue("<observation_epoch>");
    int timestamp = observation_epoch - 1230768000;
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
    char* previous_data_position;
    while (getNextHourlyData() > 0)
    {
      // set position to the hourly_data while processing an hour data chunck, as all functions use 'position'.
      previous_data_position = position;
      position = hourly_data;
      i++;
      parseHourly(i);
      position = previous_data_position;
    }
  }
}

// Main loop
while(TRUE)
{
  downloadData();
  if (DEBUG_LEVEL > 0 ) printf("%d: Wunderground xml data length: %d", debug_msg_count++, strlen(data));
  char* position = data;
  parseWeather();
  if (DEBUG_LEVEL > 0 ) printf("%d: Wunderground parsing complete", debug_msg_count++);
  sleeps(REFRESH_MIN * 60);
  if (debug_msg_count > 10000) debug_msg_count = 0;
}
