char host[50];

// Add smartsockets here:
// Do not forget to give them a Fixed IP or a DHCP reservation
char *smart_sockets[3];
smart_sockets[0]="192.168.50.28";
smart_sockets[1]="192.168.50.29";
smart_sockets[2]="192.168.50.32";

// Main Loop
while (1==1) {


  // Loop over all Smart_sockets listed
  int i = 0;
  int base=0;
  for( i = 0; i < sizeof(smart_sockets) / sizeof(smart_sockets[0]); i++)
  {

    sprintf(host, "http://admin:admin@%s", smart_sockets[i]);
    char *result;
    char *stemp = calloc(1, 10);
    char *value;

    // Get Watt usage
    result = httpget(host, "/goform/SystemCommand?command=GetInfo+W");
    if(result != 0)
    {
      free(result);
      result = httpget(host, "/adm/system_command.asp");

      value = strstrskip(result, "01W00");
      strncpy(stemp, value, 7);

      // Convert miliwatt watt into a float
      float watt=batof(stemp);
      watt=watt/100;



      // Get Voltage
      free(result);
      result = httpget(host, "/goform/SystemCommand?command=GetInfo+V");
      free(result);
      result = httpget(host, "/adm/system_command.asp");

      value = strstrskip(result, "01V00");
      strncpy(stemp, value, 7);

      // Convert voltage into a float
      float volt=batof(stemp);
      volt=volt/1000;
      free(value);
    }

    setoutput(base+1,volt);
    setoutput(base,watt);
    base=base+2;
    free(stemp);
    free(result);
  }
  sleep(5000);
} 

