#include <EtherCard.h>
#include <avr/wdt.h>
#include <MemoryFree.h>

/* 
 * Sketch to send out UDP packets to Loxone when a digital input is high
 * The structue of the UDP packet is as follows:
 *  \x00\x01\x00\x00
 *
 * First byte indicates the arduinoID
 * Second byte indicates the value of output 1
 * next byte indicates the value of output 2
 *
 * This means for digital INPUTS the hex value is /x00 or /x01
 * For analog inputs the value goes from /x00 to /xFF
 */


static byte mac[] = { 0x1A,0x2B,0x3C,0x4D,0x5E,0x6F };

/* When DHCP is not working we need to fall back to a static IP */
uint8_t ip[]      = { 192, 168, 50, 201 };    // The fallback board address.
uint8_t dns[]     = { 192, 168, 50, 1   };    // The DNS server address.
uint8_t gateway[] = { 192, 168, 50, 1   };    // The gateway router address.
uint8_t subnet[]  = { 255, 255, 255, 0  };    // The subnet mask.
byte fixed;                                   // Address fixed, no DHCP

int inputPins[]  = { 2,3,4 };  

byte Ethernet::buffer[700];
static uint32_t timer;

#define arraySize (sizeof(inputPins)/sizeof(int))
char values[arraySize+1];
unsigned long lastChange;
int maxSendTries = 2;
unsigned long maxSendDelay = 2000;
unsigned long notifyDelay = 10000; //30000 miliseconds (30 seconds)
int resetDelayOnChange = 1; //do we reset our keep-a-live counter if a change was detected


const int dstPort PROGMEM = 1234;
const int srcPort PROGMEM = 4321;

// This is the ID of the arduino, we can have up to 255 arduinos in our loxone network.
const int arduinoID PROGMEM = 0;
static byte ipLoxone[] = {192, 168, 50, 30};
static byte ipPc[] = {192, 168, 50, 61};

//the time we give the PIR sensor to calibrate (10-60 secs according to the datasheet)
//Required because otherwise we get a trigger event at startup 
int calibrationTime = 30;

//the time when the sensor outputs a low impulse
long unsigned int lowIn;

boolean lockLow = true;
boolean takeLowTime;

void setup () {
  Serial.begin(57600);

  /* Check that the Ethernet controller exists */
  /* SC pin is connected to port 10 */
  Serial.println("Initialising the Ethernet controller");
  if (ether.begin(sizeof Ethernet::buffer, mac, 10) == 0) {
    Serial.println( "Ethernet controller NOT initialised");
    while (true);
  }
  
  /* Get a DHCP connection */
  Serial.println("Attempting to get an IP address using DHCP");
  fixed = false;
  if (ether.dhcpSetup()) {
    Serial.println("Got an IP address using DHCP:");
    ether.printIp("  IP:  ", ether.myip);
    ether.printIp("  GW:  ", ether.gwip);  
    ether.printIp("  DNS: ", ether.dnsip);
  }
  /* If DHCP fails, start with a hard-coded address */
  else {
    ether.staticSetup(ip, gateway, dns);
    Serial.println("DHCP FAILED, using fixed address:");
    ether.printIp("  IP:  ", ether.myip);
    ether.printIp("  GW:  ", ether.gwip);  
    ether.printIp("  DNS: ", ether.dnsip);
    fixed = true;
  }
  
   wdt_enable(WDTO_4S);  
  // Init the PIR inputs
  for (unsigned i = 0; i < arraySize; i++) {
     pinMode(inputPins[i], INPUT);
     digitalWrite(inputPins[i], LOW);
  }
   
  //give the sensor some time to calibrate
  Serial.print("calibrating sensors ");
  for(int i = 0; i < calibrationTime; i++){
    wdt_reset();
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  values[0]=arduinoID;
  
  //all pins are set as input, get the initial values, so we can detect a change
  for (unsigned i = 0; i < arraySize; i++) {
    values[i+1] = digitalRead(inputPins[i]);
  }
  lastChange = millis();
  
  // Send initial status
  ether.sendUdp(values, sizeof(values), srcPort, ipLoxone, dstPort );
  delay(50);
  
  

}


int checkValues() {
  wdt_reset();
  int changed = 0;
  for (unsigned i = 0; i < arraySize; i++) {
    int val = digitalRead(inputPins[i]);
    if (val != values[i+1]) {
      values[i+1] = val;
      changed = 1;
    }
  }
  return changed;
}

int freeRam () {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}

void onNotifyDelay(unsigned long lastChange, unsigned long current) {
  //what should we do if there hasn't been an event for notifyDelay ms
  Serial.print(millis());
  Serial.print(" : ");
  Serial.print("No state changed so resending to be sure: -");
  for (unsigned i = 0; i < arraySize+1; i++) {
    Serial.print(values[i], DEC);
  }
  Serial.print("-\n");
  ether.sendUdp(values, sizeof(values), srcPort, ipLoxone, dstPort );
  Serial.print("freeMemory()=");
  Serial.println(freeMemory());
  Serial.print("freeRam()=");
  Serial.println(freeRam());
  
  
}

void loop () {  
  ether.packetLoop(ether.packetReceive()); // respond to incoming packets and handle ARP req
  
  if (millis() >= 18000000 ){
    delay(10000);
  }
  
  int numberOfChanges = checkValues();
  unsigned long current = millis();
  if (numberOfChanges && resetDelayOnChange) {
    Serial.print(millis());
    Serial.print(" : ");
    Serial.print("There are changes so UDP needs to be send: -");
    for (unsigned i = 0; i < arraySize+1; i++) {
      Serial.print(values[i], DEC);
    }
    Serial.print("-\n");
    ether.sendUdp(values, sizeof(values), srcPort, ipLoxone, dstPort );
    lastChange = current;
  } else {
    if (current > (lastChange+notifyDelay)) {
      onNotifyDelay(lastChange, current);
      lastChange = millis();
    }
    //if sending the keep-a-live message takes for example 10 seconds
    //start the delay from now and not from the point before we started sending
    if (numberOfChanges) {
      lastChange = millis();
    }
  }
  
  
  //ether.sendUdp(response, sizeof(response), srcPort, ipLoxone, dstPort );   
    
  
}
