/*******************************************
 * Gateway Arduino Sketch v5.1             *
 * Author: Mario Frei                      *
 * Sources: FONA library FONAtest sketch  *
 *           Xbee library example sketch   *
 *******************************************/

int gatewayID = 1;                                         // Set this accordingly
String serverURL = "http://[your url]/gateway2mysql.php";  // Instert the url to your web server here

bool resetFlag = true;

// Libraries
#include "Adafruit_FONA.h"
#include <SoftwareSerial.h>
#include <XBee.h>
#include <avr/wdt.h>  // Remember USB-Serial doesn't work after first reset.

// Debugging
#define LED_PIN 13
int counter = 0;

// FONA
#define FONA_RX 9
#define FONA_TX 8   
#define FONA_RST 4  
#define FONA_RI 7
//("gprs.swisscom.ch", "gprs", "gprs");  // Configure a GPRS APN, username, and password.
#define APN F("gprs.swisscom.ch")        // Change this according to your providers instructions
#define APN_USR F("gprs")
#define APN_PWD F("gprs")
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

String payload;




/*********************
 * Setup             *
 *********************/
void setup() {
// Initialize serial connection --------------------------------------------------------
  Serial.begin(9600);
  Serial.println("Serial initialized");
  
// Initialize Xbee ---------------------------------------------------------------------
  Serial1.begin(9600);
  xbee.setSerial(Serial1);
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

// Debugging
  Serial.println("End Setup");
  blink(10);
}


/*********************
 * Loop              *
 *********************/
void loop() {
// Debugging ---------------------------------------------------------------------------
  Serial.println(counter++);
  delay(1500);
  blink(2);
  
// Resetting ---------------------------------------------------------------------------
  if (millis()>((unsigned long)24*60*60*1000)){ // Resetting every 24 hours
    reset();
  }
// If Xbee message available, retrieve it-----------------------------------------------
    xbee.readPacket();
    if (xbee.getResponse().isAvailable()) {// got something
      if (xbee.getResponse().getApiId() == ZB_RX_RESPONSE) {// got a zb rx packet
        payload = getXbeePacket();
//  -> send to webserver ---------------------------------------------------------------
        send2Server();
        resetFlag = false;
        blink(5);
        }
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
    reset();
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

