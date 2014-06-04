int nEvents;
int n;


// Send UDP packet to wifi box,
// This function expect a code and a parameter
// The function tells the box what to do (Change color, on, off ...)
// The param tells gives the function some params
int SendUDP(int function, int param) {

  int i = 0;
  // Send every UDP command 3 times with an interval of 100ms
  // This way we are sure that the command reaches the wifi box
  do  {
    STREAM* Socket = stream_create("/dev/udp/IP-ADDRESS/8899/",0,0);
    if (Socket == NULL) {
      printf("Creating UDP socket failed");
      stream_close(Socket);
    }

    // Build the binary string to send
    char command[3]={function, param, 0x55 };

    stream_write(Socket,command,sizeof(command));
    stream_flush(Socket); stream_close(Socket);
    i = i + 1;
    sleep(100);
  } while (i < 3);
  return 1;
}

// Gets called with the group ID
// This function will parse the RGB values received from the input
// Using the RGB values we convert them to Hue and Brightness, saturation is not needed
// Since our MiLight LEDS only accept a hue value and a brightness
int SetGroup(int group, float f1) {
  char buffer[4];
  char sR[4];
  char sG[4];
  char sB[4];
  float red;
  float green;
  float blue;
  float bri;
  float r,g,b;
  float X, Y, Z, cx, cy;
  char input_color[10];
  float hue;
  float sat;
  float milightColorNo;

  sprintf(input_color, "%d", (int)f1);
  //printf("input: %d \n",(int)f1);

  // The following code parses the RGB color 
  if (strlen(input_color) == 9) {
    sB[0] = input_color[0]; sB[1] = input_color[1]; sB[2] = input_color[2]; sB[3] = '\0';
    sG[0] = input_color[3]; sG[1] = input_color[4]; sG[2] = input_color[5]; sG[3] = '\0';
    sR[0] = input_color[6]; sR[1] = input_color[7]; sR[2] = input_color[8]; sR[3] = '\0';
  }
  else if (strlen(input_color) == 8) {
    sB[0] = '0'; sB[1] = input_color[0]; sB[2] = input_color[1]; sB[3] = '\0';
    sG[0] = input_color[2]; sG[1] = input_color[3]; sG[2] = input_color[4]; sG[3] = '\0';
    sR[0] = input_color[5]; sR[1] = input_color[6]; sR[2] = input_color[7]; sR[3] = '\0';
  }
  else if (strlen(input_color) == 7) {
    sB[0] = '0'; sB[1] = '0'; sB[2] = input_color[0]; sB[3] = '\0';
    sG[0] = input_color[1]; sG[1] = input_color[2]; sG[2] = input_color[3]; sG[3] = '\0';
    sR[0] = input_color[4]; sR[1] = input_color[5]; sR[2] = input_color[6]; sR[3] = '\0';
  }
  else if (strlen(input_color) == 6) {
    sB[0] = '0'; sB[1] = '0'; sB[2] = '0'; sB[3] = '\0';
    sG[0] = input_color[0]; sG[1] = input_color[1]; sG[2] = input_color[2]; sG[3] = '\0';
    sR[0] = input_color[3]; sR[1] = input_color[4]; sR[2] = input_color[5]; sR[3] = '\0';
  }
  else if (strlen(input_color) == 5) {
    sB[0] = '0'; sB[1] = '0'; sB[2] = '0'; sB[3] = '\0';
    sG[0] = '0'; sG[1] = input_color[0]; sG[2] = input_color[1]; sG[3] = '\0';
    sR[0] = input_color[2]; sR[1] = input_color[3]; sR[2] = input_color[4]; sR[3] = '\0';
  }
  else if (strlen(input_color) == 4) {
    sB[0] = '0'; sB[1] = '0'; sB[2] = '0'; sB[3] = '\0';
    sG[0] = '0'; sG[1] = '0'; sG[2] = input_color[0]; sG[3] = '\0';
    sR[0] = input_color[1]; sR[1] = input_color[2]; sR[2] = input_color[3]; sR[3] = '\0';
  }
  else if (strlen(input_color) == 3) {
    sB[0] = '0'; sB[1] = '0'; sB[2] = '0'; sB[3] = '\0';
    sG[0] = '0'; sG[1] = '0'; sG[2] = '0'; sG[3] = '\0';
    sR[0] = input_color[0]; sR[1] = input_color[1]; sR[2] = input_color[2]; sR[3] = '\0';
  }
  else if (strlen(input_color) == 2) {
    sB[0] = '0'; sB[1] = '0'; sB[2] = '0'; sB[3] = '\0';
    sG[0] = '0'; sG[1] = '0'; sG[2] = '0'; sG[3] = '\0';
    sR[0] = '0'; sR[1] = input_color[0]; sR[2] = input_color[1]; sR[3] = '\0';
  }
  else if (strlen(input_color) == 1) {
    sB[0] = '0'; sB[1] = '0'; sB[2] = '0'; sB[3] = '\0';
    sG[0] = '0'; sG[1] = '0'; sG[2] = '0'; sG[3] = '\0';
    sR[0] = '0'; sR[1] = '0'; sR[2] = input_color[0]; sR[3] = '\0';
  }

  // Convert the strings back to int
  red = atoi(sR);
  green = atoi(sG);
  blue = atoi(sB);
  printf("red: %d \n",red);
  printf("green: %d \n",green);
  printf("blue: %d \n",blue);


   // Clear older hue and sat values
   hue = 0;
   sat = 0;

  // Convert based on RGB levels to HUE and Saturation
  if ((red >= green) && (green >= blue)) {
    hue = 60*(green-blue)/(red-blue);
    sat = (red - blue) / red;
  } else if ((green > red) && (red >= blue)) {
    hue = 60*(2 - (red-blue)/(green-blue));
    sat = (green - blue) / green;
  } else if ((green >= blue) && (blue > red)) {
    hue = 60*(2 + (blue-red)/(green-red));
    sat = (green - red) / green;
  } else if ((blue > green) && (green > red)) {
    hue = 60*(4 - (green-red)/(blue-red));
    sat = (blue - red) / blue;
  } else if ((blue > red) && (red >= green)) {
    hue = 60*(4 + (red-green)/(blue-green));
    sat = (blue - green) / blue;
  } else if ((red >= blue) && (blue > green)) {
    hue = 60*(6 - (blue-green)/(red-green));
    sat = (red - green) / red;
  }

  sat = sat * 255;


  // set for safety again saturated at white
  if (blue == 0 && green == 0 && red == 0) {
    sat = 0;
  }

  // First map the 360 degrees value to a value of 0-255
  milightColorNo =(176-(hue/360.0*255.0));
  if ( milightColorNo < 0 ) {
    milightColorNo = 256+ milightColorNo;
  }

  // Convert the saturation to the brightness
  bri = sqrt(pow(red, 2) + pow(green, 2) + pow(blue, 2));
  bri = bri * 2;
  if (bri > 255) {
    bri = 255;
  }
  // Falls nur 1 Farbe ein Thema, Helligkeit=Farbe
  if (red == 0 && green == 0) {
    bri = (blue * 255)/100;
  }

  if (blue == 0 && green == 0)
    bri = (red * 255)/100;

  if (red == 0 && blue == 0)
    bri = (green * 255)/100;


  // Milight only supports brightness levels from 2-27, so scale 0-255 to 0-27
  bri=bri/255.0*27.0;
  if ( bri < 3) {
    bri=2;
  }

  // If all colors are the same level we get white, so set de RGB strip into white mode
  if (red > 0 || green > 0 || blue > 0 ) {
    if (red == green && green == blue && blue == red ) {
      if (group == 1) {
        // Set white mode for group 1
        SendUDP( 0xC5, 0x00 );
      }
      if (group == 2) {
        // Set white mode for group 2
        SendUDP( 0xC7, 0x00 );
      }
      if (group == 3) {
        // Set white mode for group 3
        SendUDP( 0xC9, 0x00 );
      }
      if (group == 4) {
        // Set white mode for group 4
        SendUDP( 0xCB, 0x00 );
      }

      // Set brightness (Will be executed only for the group selected above)
      SendUDP( 0x4E, (int) bri );

      } else {
        if (group == 1) {
          // Group 1 on and if already on, select it
          SendUDP( 0x45, 0x00 );
        }
        if (group == 2) {
          // Group 2 on and if already on, select it
          SendUDP( 0x47, 0x00 );
        }
        if (group == 3) {
          // Group 3 on and if already on, select it
          SendUDP( 0x49, 0x00 );
        }
        if (group == 4) {
          // Group 4 on and if already on, select it
          SendUDP( 0x4B, 0x00 );
        }
        // Set hue color (Will be executed only for the group selected above)
        SendUDP( 0x40, milightColorNo );
        // Set brightness (Will be executed only for the group selected above)
        SendUDP( 0x4E, (int) bri );
      }
    } else {
      if (group == 1) {
        // Set group 1 off
        SendUDP( 0x46, 0x00 );
      }
      if (group == 2) {
        // Set group 2 off
        SendUDP( 0x48, 0x00 );
      }
      if (group == 3) {
        // Set group 3 off
        SendUDP( 0x4A, 0x00 );
      }
      if (group == 4) {
        // Set group 4 off
        SendUDP( 0x4C, 0x00 );
      }
    }
    return 1;
}


// Main Loop
while (1==1) {

  nEvents = getinputevent();

  // Test bitwise if input 3 was changed (00000100)
  if (nEvents & 0x04) {
    SetGroup(1, getinput(0));
  }
  // Test bitwise if input 4 is changed (00001000)
  if (nEvents & 0x08) {
    SetGroup(2, getinput(1));
  }
  // Test bitwise if input 5 is changed (00010000)
  if (nEvents & 0x10) {
    SetGroup(3, getinput(2));
  }
  // Test bitwise if input 6 is changed (00100000)
  if (nEvents & 0x20) {
    SetGroup(4, getinput(3));
  }

  sleep(500);
}

