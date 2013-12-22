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
#define REFRESH_MIN 15 // refresh elke 15 minuten (developer account kan 500 calls per dag doen; =20 calls per uur)

// Settings
// create yourself a (free) developer account on wunderground website; it can handdle up to 500 calls/day for free.
char* apiKey   = "f5828ac38f534229"; 	// your personal 8 hex characters api key from your account at Wunderground (need to request this key once !)
char* location = "51.2913,4.94935"; 	// your location
char* host     = "api.wunderground.com";



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
  if (strcmp(code, "Clear") == 0) return 2;
  if (strcmp(code, "Scattered Clouds") == 0) return 3;
  if (strcmp(code, "Partly Cloudy") == 0) return 3;
  if (strcmp(code, "Mostly Cloudy") == 0) return 4;
  if (strcmp(code, "Mostly Sunny") == 0) return 1;
  if (strcmp(code, "Cloudy") == 0) return 5;
  if (strcmp(code, "Overcast") == 0) return 7;
  if (strcmp(code, "Small Hail") == 0) return 25;
  if (strfind(code, "Heavy Freezing Rain", 0) >= 0) return 15;
  if (strfind(code, "Heavy Rain Showers", 0) >= 0) return 17;
  if (strfind(code, "Rain Showers", 0) >= 0) return 16;
  if (strfind(code, "Freezing Rain", 0) >= 0) return 14;
  if (strfind(code, "Light Rain", 0) >= 0) return 10;
  if (strfind(code, "Heavy Rain", 0) >= 0) return 12;
  if (strfind(code, "Heavy Sleet", 0) >= 0) return 27;
  if (strfind(code, "Light Sleet", 0) >= 0) return 25;
  if (strfind(code, "Sleet", 0) >= 0) return 26;
  if (strfind(code, "Heavy Thunderstorm", 0) >= 0) return 19;
  if (strfind(code, "Thunderstorm", 0) >= 0) return 18;
  if (strfind(code, "Heavy Snow Showers", 0) >= 0) return 24;
  if (strfind(code, "Snow Showers", 0) >= 0) return 23;
  if (strfind(code, "Rain", 0) >= 0) return 11;
  if (strfind(code, "Heavy Snow", 0) >= 0) return 22;
  if (strfind(code, "Light Snow", 0) >= 0) return 20;
  if (strfind(code, "Snow", 0) >= 0) return 21;
  if (strfind(code, "Drizzle", 0) >= 0) return 13;
  if (strfind(code, "Fog", 0) >= 0) return 6;
  if (strfind(code, "Sunny", 0) >= 0) return 2;
  //	printf("No weather code mapping for %s", code);
  return -1;
}

void parseHourly()
{
  int timestamp = readIntValue("<epoch>") - 1230768000;
  if (moveToKey("<temp>")) setweatherdata(1, timestamp, readFloatValue("<metric>"));
  if (moveToKey("<dewpoint>")) setweatherdata(2, timestamp, readFloatValue("<metric>"));
  int weatherCode = parseWeatherCode(readStringValue("<condition>", 0));
  if (weatherCode > 0) setweatherdata(10, timestamp, weatherCode);
  if (moveToKey("<wspd>")) setweatherdata(4, timestamp, readFloatValue("<metric>"));
  if (moveToKey("<wdir>")) setweatherdata(5, timestamp, readIntValue("<degrees>"));
  setweatherdata(3, timestamp, readIntValue("<humidity>"));
  if (moveToKey("<feelslike>")) setweatherdata(26, timestamp, readFloatValue("<metric>"));
  if (moveToKey("<mslp>")) setweatherdata(11, timestamp, readIntValue("<metric>"));
}

void parseWeather()
{
  // Current observation
  if (moveToKey("<current_observation>") > 0) 
  {
    // nog iets serieus fout met inlezen van die waarden; vindt die niet terug in de UI, en ook foute data.
    printf("Wunderground values <observation_time>: %s" ,readStringValue("<observation_time>",0));
    int timestamp = readIntValue("<observation_epoch>") - 1230768000;
    int weatherCode = parseWeatherCode(readStringValue("<weather>", 0));
    if (weatherCode > 0) setweatherdata(10, timestamp, weatherCode);
    setweatherdata(1, timestamp, readFloatValue("<temp_c>"));
    setweatherdata(3, timestamp, readPercentageValue("<relative_humidity>"));
    setweatherdata(5, timestamp, readIntValue("<wind_degrees>"));
    setweatherdata(4, timestamp, readFloatValue("<wind_kph>"));
    setweatherdata(11, timestamp, readIntValue("<pressure_mb>"));
    setweatherdata(2, timestamp, readFloatValue("<dewpoint_c>"));
    setweatherdata(26, timestamp, readFloatValue("<feelslike_c>"));
    setweatherdata(9, timestamp, readFloatValue("<precip_today_metric>"));
    setweatherdata(22, timestamp, getcurrenttime());
    // 			setweatherdata(23, timestamp, getcurrenttime());
    setweatherdata(23, timestamp, timestamp);
    setweatherdata(12, timestamp, readIntValue("<observation_epoch>"));
  }
  // Hourly forecasts
  if (moveToKey("<hourly_forecast>") > 0)
  {
    while (moveToKey("<forecast>") > 0)
    {
      parseHourly();
    }
  }
}


// Main loop
//printf("Om Wunderground.com weer service te doen werken moet de lijn char *data = downloadData() uit commentaar gehaald worden.");
while(TRUE)
{
  char* data = downloadData();
  printf("Wunderground xml data length: %d", strlen(data));
  position = data;
  parseWeather();
  free(data);
  // refresh elke REFRESH_MIN minuten
  // vb REFRESH_MIN = 15: wunderground.com developer account kan 500 calls/day; 15/min = 20/h = 480/d.
  sleeps(REFRESH_MIN * 60);
}
