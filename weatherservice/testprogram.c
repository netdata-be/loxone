//
// This is just a test program which sets the temperature to a certain value
// When viewing the weatherdata from the app or from the website it seems to work
// This means that the temperature is updated every 60 seconds
//
// However when you use the weatherdata in your loxone program
// The temperature value never get's updated.
//
// I also set the custom1 field to the current timestamp to verify when the latest update was done

int i=0;
while(TRUE)
{
  i++;
  int timestamp = getcurrenttime();
  printf("weatherservice [INFO]: Testrun, setting temp to %d at timestamp %d", i, timestamp );
  setweatherdata(1,  timestamp, i);
  setweatherdata(12,  timestamp, timestamp);
  sleeps(60);
}
