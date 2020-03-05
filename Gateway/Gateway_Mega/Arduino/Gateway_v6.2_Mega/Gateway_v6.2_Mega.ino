/***************************************************
 * Gateway Arduino Sketch v6.2                     *
 * Author: Mario Frei                              *
 * Year: 2019                                      *
 * Sources: FONA library FONAtest sketch           *
 *          Xbee library example sketch            *
 *          SD library example sketch              *
 *          RTC library example sketch (Sparkfun)  * 
 ***************************************************/

// Improvement from 6.1
//  - Add fona RSSI readout               ()
//  - Add fona battery readout            ()
//
// To Do:
//  - Add heart beat message              ()
//  - Add input parameters to send2server ()
//  - Add input parameters to write2sd    ()

int gatewayID = 0;                                                   // Set this accordingly
String serverURL = "http://[your webserver]/wsn/gateway2mysql.php";  // Insert the url to your web server here
float heartbeat_interval = 0.5;                                      // Heartbeat interval in hours
bool resetFlag = true;

// Libraries
#include "Adafruit_FONA.h"     // FONA
#include <SoftwareSerial.h>    // FONA
#include <XBee.h>              // Xbee
#include <avr/wdt.h>           // Reset: Remember USB-Serial doesn't work after first reset.
#include <SparkFunDS1307RTC.h> // RTC
#include <Wire.h>              // RTC
#include <SPI.h>               // SD card
#include <SD.h>                // SD card
#include <PayloadClass.h>      // Payload class library

// Debugging
#define LED_PIN 3
int counter = 0;
long timestamp = 0;

// FONA
#define FONA_RX 2
#define FONA_TX 10   
#define FONA_RST 22  
#define FONA_RI 53
//("gprs.swisscom.ch", "gprs", "gprs");  // Configure a GPRS APN, username, and password.
#define APN F("gprs.swisscom.ch")        // Change this according to your providers instructions
#define APN_USR F("gprs")                // Change this according to your providers instructions
#define APN_PWD F("gprs")                // Change this according to your providers instructions 
SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
SoftwareSerial *fonaSerial = &fonaSS;
Adafruit_FONA fona = Adafruit_FONA(FONA_RST);// Use this one for FONA 3G: //Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);
uint8_t readline(char *buff, uint8_t maxbuff, uint16_t timeout = 0);

// Xbee
XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
ZBRxResponse rx = ZBRxResponse();
ModemStatusResponse msr = ModemStatusResponse();

// SD card
const int chipSelect = 8;

// Other
String payload;
bool sd_ignore_flag = 0;
int gprs_fail_counter = 0;


/*********************
 * Setup             *
 *********************/
void setup() {
// Initialize serial connection --------------------------------------------------------
  Serial.begin(9600);
  Serial.println("Serial initialized");
  
// Initialize Xbee ---------------------------------------------------------------------
  Serial3.begin(9600);
  xbee.setSerial(Serial3);
  Serial.println("Xbee initialized");
  
// Initialize Fona ---------------------------------------------------------------------
  fonaSerial->begin(4800);
  if (! fona.begin(*fonaSerial)) {
    Serial.println("FONA not OK");
    while (1);
  }
  Serial.println("FONA OK");
  Serial.println("Wait for Call Ready & SMS Ready messages");
  delay(14000); // Wait for "Call Ready" & "SMS Ready" messages
  while (fona.available()) {
      Serial.write(fona.read());
  }
  // turn GPRS on
  startGPRS();
  Serial.println("FONA initialized");

// Initialize RTC  --------------------------------------------------------------------
  rtc.begin(); // Call rtc.begin() to initialize the library
  rtc.set24Hour(); // Use rtc.set24Hour to set to 24-hour mode
  Serial.println("RTC initialized.");

// Initialize SD card  ----------------------------------------------------------------
  if (!SD.begin(chipSelect)) {
    Serial.println("SD card failed, or not present");
    sd_ignore_flag = 1;
  } else {
    Serial.println("SD card initialized.");
  }

// Debugging
  Serial.println("End Setup");
  blink(10);
}


/*********************
 * Loop              *
 *********************/
void loop() {
// Debugging ---------------------------------------------------------------------------
  timestamp = millis()/1000;
  Serial.print(counter++);
  Serial.print(": ");
  Serial.println(timestamp);
  delay(1500);
  blink(2);
  
// If Xbee message available, retrieve it-----------------------------------------------
  xbee.readPacket();
  if (xbee.getResponse().isAvailable()) {// got something
    if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {// got a zb rx packet
      payload = getXbeePacket();
//  -> send to webserver ---------------------------------------------------------------
      send2Server();
      resetFlag = false;
      blink(5);
//  -> Write to SD card ----------------------------------------------------------------
      write2SD();
      }
  }

// If it is time send heartbeat message
  if (timestamp>(heartbeat_interval*3600)){
    heartbeat();
    Serial.println("Send heartbeat message");
  }
  
// Reset: If send2Server failed more than 100 times reset gateway.
//        If running for longer than 24 hours
  if (gprs_fail_counter>5){
    Serial.print("gprs_fail_counter =");
    Serial.println(gprs_fail_counter);
    reset();
  }
  if (timestamp>24*3600){
    Serial.print("millis()=");
    Serial.println(millis());
    reset();
  }
}


/*********************************************
 * Retrieve Xbee packet                      *
 *********************************************/
String getXbeePacket(){
    // now fill our zb rx class
    xbee.getResponse().getZBRxResponse(rx);   
    unsigned char payload[rx.getDataLength()];      
    for (int i = 0; i < rx.getDataLength(); i++) {
      payload[i] = char(rx.getData()[i]);
    }
    char payloadCharArray[sizeof(payload)*3+1];
    for (int i=0; i < sizeof(payload); i++){
      hex2char(payloadCharArray, i*3, byte(payload[i]));
      payloadCharArray[i*3+2] = '.';
    }
    payloadCharArray[sizeof(payloadCharArray)-1] = '\0';
    return payloadCharArray;
}


/*********************************************
 * Send2Server                               *
 *********************************************/
bool send2Server()
{
  uint16_t statuscode;
  int16_t length;
  String url; 
  url  = serverURL;
  url += "?payload=";
  url += payload;
  url += "&g_id=";
  url += gatewayID;
  if (resetFlag){
    url += "&comment=Reset";
  }
  
  char urlCharArray [url.length()+1];
  url.toCharArray(urlCharArray, url.length()+1);
  
  if (!fona.HTTP_GET_start(urlCharArray, &statuscode, (uint16_t *)&length)) {
    // Failed to send: Resetting uC, FONA library will reset FONA
    gprs_fail_counter++;
    Serial.print("GPRS failed: gprs_fail_counter=");
    Serial.println(gprs_fail_counter);
  }
  else {
    fona.HTTP_GET_end();
  }
  return true;
}


/*********************************************
 * hex2char                                  *
 *********************************************/
void hex2char(char* charArray, int index, byte theNumber){
  charArray[index] = (theNumber >> 4) + 0x30;
  if (charArray[index] > 0x39) charArray[index] +=7;
  charArray[index+1] = (theNumber & 0x0f) + 0x30;
  if (charArray[index+1] > 0x39) charArray[index+1] +=7;
  return;
}


/**************************************************
 * enable GPRS                                    *
 **************************************************/
void startGPRS(){
  fona.setGPRSNetworkSettings(APN, APN_USR, APN_PWD);  // Configure a GPRS APN, username, and password.
  if (!fona.enableGPRS(true))
  {
    Serial.println(F("GPRS failed"));
  } else{
    Serial.println(F("GPRS on"));
  }
}

 
/**************************************************
 * LED Blink                                      *
 **************************************************/
void blink(int n){
  int T = 75;
  for (int i=0; i<n; i++){
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, HIGH);
    delay(T);
    digitalWrite(LED_PIN, LOW);
    delay(T);
  }
}


/**************************************************
 * Reset                                          *
 *************************************************/
// Remember, the USB-serial doesn't work anymore after first reset.
void reset(){
  // Turn FONA library does a hard reset, therefore resetting FONA seperatly is not necessary
  Serial.println(F("Resetting"));
  // Reset Microcontroller
  wdt_enable(WDTO_15MS);         // WDTO_15MS, 30MS, 60MS, 120MS, 250MS, 500MS, 1S, 2S, 4S, 8S
}



/**************************************************
 * Write to SD card                               *
 **************************************************/
void write2SD(){
  if (sd_ignore_flag ==1 ) {
    Serial.println("Ignore SD card");
    return;
  } else {
    Serial.println("Write to SD card");
  }
  
  // make a string for assembling the data to log:
  String dataString = "";
  dataString += getRTCdate();
  dataString += " - ";
  dataString += payload;
  dataString += "\n";
  // Actually write to SD card
  File dataFile = SD.open("log.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening log.txt");
  }
}


/**************************************************
 * Get date                                       *
 **************************************************/
String getRTCdate(){
  Serial.println("Get date from RTC");
  String myDate = "";
  if(rtc.update()) {
    myDate += String(rtc.hour());
    myDate += ":";
    myDate += String(rtc.minute());
    myDate += ":";
    myDate += String(rtc.second());
    myDate += " ";
    myDate += String(rtc.date());
    myDate += ".";
    myDate += String(rtc.month());
    myDate += ".";
    myDate += String(rtc.year());
  } else {
    // RTC error
    myDate += "RTC error";
  }
  return myDate;
}


/**************************************************
 * Get RSSI from modem                            *
 **************************************************/
int8_t fona_get_rssi(){
  uint8_t n = fona.getRSSI();
  int8_t r;

  Serial.print(F("RSSI = ")); Serial.print(n); Serial.print(": ");
  if (n == 0) r = -115;
  if (n == 1) r = -111;
  if (n == 31) r = -52;
  if ((n >= 2) && (n <= 30)) {
    r = map(n, 2, 30, -110, -54);
  }
  return r;
}


/**************************************************
 * Get battery voltage from fona                  *
 **************************************************/
uint16_t fona_get_bat(){
  uint16_t vbat;
  fona.getBattPercent(&vbat);
  return vbat;
}


/**************************************************
 * Heartbeat                                      *
 **************************************************/

bool heartbeat(){
  // Read out data
  float myRSSI = fona_get_rssi();
  float myBat  = fona_get_bat();

  // Add data to standard payload
  PayloadClass myMessage;
  myMessage.set_id(gatewayID*100);
  myMessage.set_SMtype(20);
  myMessage.set_payload(myBat, myRSSI, 0, 0, 0); 

  // Convert standard payload to char array with dots for separation
  unsigned char payload[size_of(myMessage.payload)];      
  for (int i = 0; i < rx.getDataLength(); i++) {
    payload[i] = char(myMessage.payload[i]);
  }
  char payloadCharArray[sizeof(payload)*3+1];
  for (int i=0; i < sizeof(payload); i++){
    hex2char(payloadCharArray, i*3, byte(payload[i]));
    payloadCharArray[i*3+2] = '.';
  }
  payloadCharArray[sizeof(payloadCharArray)-1] = '\0';
  payloadCharArray;

  // Add payload to url for http request to webserver
  uint16_t statuscode;
  int16_t length;
  String url; 
  url  = serverURL;
  url += "?payload=";
  url += payloadCharArray;
  url += "&g_id=";
  url += gatewayID;
  char urlCharArray [url.length()+1];
  url.toCharArray(urlCharArray, url.length()+1);
  
  // Send payload char array to webserver
  if (!fona.HTTP_GET_start(urlCharArray, &statuscode, (uint16_t *)&length)) {
    // Failed to send: Resetting uC, FONA library will reset FONA
    gprs_fail_counter++;
    Serial.print("GPRS failed: gprs_fail_counter=");
    Serial.println(gprs_fail_counter);
  }
  else {
    fona.HTTP_GET_end();
  }
  return true;
}
