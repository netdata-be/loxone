#define MAX_BUFF 80001
#define MAX_FIELDS 20
#define RD_BLOCK_SIZE 128
#define DEBUG_LEVEL 1  // 0 = disable ; 1 = INFO ; 2 = DEBUG

char buffer[MAX_BUFF];

int downloadData()
{
  char device[255];
  sprintf(device, "/dev/tcp/%s/80", "netdata.be");
  char headers[255];
  sprintf(headers, "GET /loxone/test2.php HTTP/1.1\r\nHost: %s\r\nUser-Agent: LoxLIVE [en]\r\nContent-Type: text/html; charset=utf-8\r\n\r\n",  "netdata.be");
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
    if (count > 0) strncpy((char*)buffer + i * RD_BLOCK_SIZE, block, count);
    i++;
    if (i >= ( ( MAX_BUFF - 1 ) / RD_BLOCK_SIZE )) count=0; // avoid buffer overflows

  }
  while (count > 0);
  stream_close(tcpStream);
  buffer[MAX_BUFF - 1] = 0; //put null character or end of string at the end.
  return 0;
}

char *fieldStart[MAX_FIELDS];

//takes the start of a string and tries to parse elements seperated by a ':'
//return the number of elements parsed, -1 on error
static int parseLine(char **start) {
  char *buff = *start;
  int fieldIndex = 0;
  if (*buff == '\n' || *buff == '\0') {
    fieldStart[fieldIndex] = NULL;
    return 0;
  }

  fieldStart[fieldIndex] = buff;
  ++fieldIndex;

  //while the line is not processed
  while (*buff != '\n' && *buff != '\0') {
    if (*buff == ':') {
      *buff = '\0';
      if (fieldIndex < MAX_FIELDS) {
        fieldStart[fieldIndex] = buff+1;
      }
      ++fieldIndex;
    }
    buff++;
  }

  //we came accross the newline or buffer end
  if (*buff == '\n') {
    *buff = '\0';
    *start = buff+1;
  } else {
    *start = NULL;
  }
  if (fieldIndex <= MAX_FIELDS)
    return fieldIndex;
  return -1;
}


static void parseWeatherData(char **fields, int count) {
  char setioVarName[50];
  if (DEBUG_LEVEL > 0 ) printf("weatherservice [INFO]: Getting forecast for +%s hour ,received %d elements\n", fieldStart[0], count);
  int i = 0;
  if (count == 15) {
    int offset = atoi(fields[0]);
    if ( offset == 0 ) {
      int timestamp = getcurrenttime();
      int timestamp_real = atoi(fields[1]);
      setweatherdata(22, timestamp, timestamp);
      setweatherdata(23, timestamp, timestamp_real);
    } else {
      int timestamp = atoi(fields[1]);
    }
    // FIELD | NAME                | WeatherID |
    // ------+---------------------+-----------+
    //  0    | offset              |           |
    //  1    | epoch               |           |
    //  2    | temp_c              | 1         |
    //  3    | relative_humidity   | 3         |
    //  4    | wind_dir            |           |
    //  5    | wind_degrees        | 5         |
    //  6    | wind_kph            | 4         |
    //  7    | wind_gust_kph       | 6         |
    //  8    | pressure_mb         | 11        |
    //  9    | dewpoint_c          | 2         |
    // 10    | windchill_c         |           |
    // 11    | feelslike_c         | 26        |
    // 12    | solarradiation      | 27 , 7    |
    // 13    | precip_today_metric | 9         |
    // 14    | weathercode         | 10        |
    // ------+---------------------+-----------+

    if (*fields[2] != '\0') {
      setweatherdata(1,  timestamp, atof(fields[2]) );
      sprintf(setioVarName, "outsideTemp_offset_hour_%d", offset);
      setio(setioVarName, atof(fields[2]));
    }
    if (*fields[3] != '\0') {
      setweatherdata(3,  timestamp, atof(fields[3]) );
    }
    if (*fields[5] != '\0') {
      setweatherdata(5,  timestamp, atof(fields[5]) );
    }
    if (*fields[6] != '\0') {
      setweatherdata(4,  timestamp, atof(fields[6]) );
    }
    if (*fields[7] != '\0') {
      setweatherdata(6,  timestamp, atof(fields[7]) );
    }
    if (*fields[8] != '\0') {
      setweatherdata(11,  timestamp, atof(fields[8]) );
    }
    if (*fields[9] != '\0') {
      setweatherdata(2,  timestamp, atof(fields[9]) );
    }
    if (*fields[11] != '\0') {
      setweatherdata(26,  timestamp, atof(fields[11]) );
    }
    if (*fields[12] != '\0') {
      setweatherdata(7,  timestamp, atof(fields[12]) );
      setweatherdata(27,  timestamp, atof(fields[12]) );
      sprintf(setioVarName, "solar_radiation_offset_hour_%d", offset);
      setio(setioVarName, atof(fields[12]));

    }
    if (*fields[13] != '\0') {
      setweatherdata(9,  timestamp, atof(fields[13]) );
    }
    if (*fields[14] != '\0') {
      setweatherdata(10,  timestamp, atof(fields[14]) );
    }
  } else {
    printf("Only received %d items", count);
  }
}

setio("weather_status", 2);

// Main loop
while(TRUE)
{
  setio("weather_status", 1);
  downloadData();
  printf("weatherservice [INFO]: xml data length: %d", strlen(buffer));
  char* csv_data = strstrskip(buffer,"\r\n\r\n");

  char *start = &csv_data[0];
  int elems = 0;
  while ((elems = parseLine(&start)) > 0) {
    parseWeatherData(&fieldStart[0], elems);
    if (start == NULL) {
      printf("weatherservice [ERR]: WAAH, Could not loop over elements => skipping this run.\n");
      break;
    }
  }
  if (elems < 0) {
    printf("weatherservice [ERR]: WAAH, error while parsing line.\n");
  }
  setio("weather_status", 0);
  sleeps(200);
}


