#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 3

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

// arrays to hold device address
DeviceAddress insideThermometer;

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
byte server[] = { 42, 159, 133, 93 };
byte ip[]     = { 192, 168, 1, 105 };
byte dn[]     = { 8, 8, 8, 8 };
byte gw[]     = { 192, 168, 1, 1 };

int led = 2;

int p = 1; // Callback function header
void callback(char* topic, byte* payload, unsigned int length);

EthernetClient ethClient;
PubSubClient client(server, 1883, callback, ethClient);

// Callback function
void callback(char* topic, byte* payload, unsigned int length) {
  // In order to republish this payload, a copy must be made
  // as the orignal payload buffer will be overwritten whilst
  // constructing the PUBLISH packet.
  
  // Allocate the correct amount of memory for the payload copy
  byte* p = (byte*)malloc(length);
  // Copy the payload to the new buffer
  memcpy(p,payload,length);
  int a = p[0];
   Serial.print(a);
  if (a==48)
  {
  digitalWrite(led, LOW); 
  }
  else
  {
  digitalWrite(led, HIGH); 
  }
  free(p);
}

void setup()
{
   pinMode(led, OUTPUT);  
   pinMode(p, OUTPUT);    
   digitalWrite(led, HIGH); 
   digitalWrite(p, HIGH);    
 // Ethernet.begin(mac, ip,dn,gw);
   Ethernet.begin(mac);
   int i = 1;
   while (i)
    {
      if (client.connect("arduinoClient")) {
        client.subscribe("begin");
        i = 0;
      }
  }
  
  // start serial port
  Serial.begin(9600);
  Serial.println("Dallas Temperature IC Control Library Demo");

  // locate devices on the bus
  Serial.print("Locating devices...");
  sensors.begin();
  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  // report parasite power requirements
  Serial.print("Parasite power is: "); 
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");
  

  if (!sensors.getAddress(insideThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  

  // show the addresses we found on the bus
  Serial.print("Device 0 Address: ");
  //printAddress(insideThermometer);
  Serial.println();

  // set the resolution to 9 bit (Each Dallas/Maxim device is capable of several different resolutions)
  sensors.setResolution(insideThermometer, 9);
 
  Serial.print("Device 0 Resolution: ");
  Serial.print(sensors.getResolution(insideThermometer), DEC); 
  Serial.println();  
}

void loop()
{
  sensors.requestTemperatures(); 
  float tempC = sensors.getTempC(insideThermometer);
  Serial.print(tempC);
  char s[32]; 
  client.publish("app",dtostrf(tempC, 2, 1, s));
  delay(1000);  
  client.loop();
}
