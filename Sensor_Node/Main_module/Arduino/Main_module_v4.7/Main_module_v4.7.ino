// Main module v4.7

// Improvments from v2
//  - remove sd card functionality                         (tested)
//  - remove c_SD_XBEE_EVERY, remove measurment_counter    (tested)
//  - remove code that was outcommented                    (tested)
//  - remove readDS3231time                                (tested)
//  - Reformat {}                                          (tested)
//  - merge if(debug_enable=True) statements               (tested)
//  - add blink function                                   (tested)
//  - Rename C_MEASUREMENT_EVERY_X_MIN TO SAMPING_INTERVAL (tested)
// Improvements from v4.4
//  - Implement universal message library                  (tested)
//  - Implement heartbeat                                  (tested)
//  - Change battery warning message to universal format   (tested)
//  - Encapsulate battery check and alarm in function      (tested)
// Improvements from v4.5
//  - Move includes up                                     (tested)
//  - Remove xbee_transmit_lowbatterywarning               (tested)
//  - Get RSSI from XBee for heartbeat message             (tested)
//  - Web parser fixed (isituprightnow_v5.php)             (tested)
// Improvements from v4.6
//  - Change LED on time from 2s to 1s                     (tested)
//  - Fix reporting interval of heartbeat message          ()
//  - Change heartbeat interval according to debug mode    ()
//  - Add new low battery check to server                  ()
//  - 1272 -> 780 lines

// To Do:
//  - Apply payload class to standard measurement payload
//  - Improve terminology of Payload class (message vs payload vs myMessage vs myPayload)
//  - Use watchdog timer for wake up
//  - Implement universal coordinator address
//  - Test current draw
//  - Test low battery functionality
//  - Fix ddrDB2 (now there are multiple message types per node)

/********************************
   CONFIGURATION
 ********************************/
static const uint16_t C_SAMPLING_INTERVAL       = 5;    // Every how many minutes a measurement is taken from the sensor module
static const float    LOW_BATTERY_WARNING_LEVEL = 3.4;  // V
static const uint16_t C_HEARTBEAT_INTERVAL      = 2*60; // Every how many minutes a heartbeat signal is sent

// Coordinator XBee Address:
#define UPLINK_SH 0x13A200

//#define UPLINK_SL 0x          // Cxxx
#define UPLINK_SL 0x41046BD5  // C100
//#define UPLINK_SL 0x41628F9C  // C200
//#define UPLINK_SL 0x415ABDD9  // C300
//#define UPLINK_SL 0x41046775  // C400
//#define UPLINK_SL 0x41631484  // C500
//#define UPLINK_SL 0x41628FF0  // C600
//#define UPLINK_SL 0x41628FA9  // C700
//#define UPLINK_SL 0x41628FFD  // C800

// Network ID (XBee PAN ID of Corrdinator): unsigned int 32 bit
//static const uint32_t PAN_ID = 0x01;   // C0
static const uint32_t PAN_ID = 0x100;  // C100
//static const uint32_t PAN_ID = 0x200;  // C200
//static const uint32_t PAN_ID = 0x300;  // C300
//static const uint32_t PAN_ID = 0x400;  // C400
//static const uint32_t PAN_ID = 0x500;  // C500
//static const uint32_t PAN_ID = 0x600;  // C600
//static const uint32_t PAN_ID = 0x700;  // C700
//static const uint32_t PAN_ID = 0x800;  // C800

// This communication module's ID. Allowed range: [0,65535] (unsigned 16 bit integer)
static const uint16_t THIS_CM_ID = 115;
/********************************
   END OF CONFIGURATION
 ********************************/
#include <avr/interrupt.h>
#include <avr/power.h>
#include <avr/sleep.h>
#include <avr/io.h>
#include "Wire.h"
#define DS3231_I2C_ADDRESS 0x68
#include <SPI.h>
#include <XBee.h>
#include <SoftwareSerial.h>
#include <PayloadClass.h>
 
uint16_t             wakeup_counter          = 0;
uint16_t             heartbeat_counter       = 0;
static const uint8_t FRAMESTART_BYTE         = 0xAA; // ANYTHING OTHER THAN 0xBB
uint8_t*             total_xbee_payload;
uint8_t              total_xbee_payload_size;
PayloadClass         myPayload;
uint8_t              SMtype                  = 0; // Will be set once the sensor module tells us
uint8_t              sizeofSMdata            = 0; // Will be set to number of bytes of the SM payload once the sensor module
uint16_t             SAMPLING_INTERVAL       = 1;
uint16_t             HEARTBEAT_INTERVAL      = 1;
static const uint8_t xbee_header_bytes       = 4; // uint16_t NodeID, uint8_t n_measurements, uint8_t SM_type

// Pin numbers
static const uint8_t P_L1           = 14; // LED 1
static const uint8_t P_L2           = 15; // LED 2
static const uint8_t P_L3           = 16; // LED 3
static const uint8_t P_DBG_ENABLE   = 17; // Input, Debug Slide Switch
static const uint8_t P_SDA          = 18; // SDA pin for I2C
static const uint8_t P_SCL          = 19; // SDL pin for I2C
static const uint8_t P_VBAT_MEASURE = 20; // Input, pin for measureing battery voltage over voltage divider
static const uint8_t P_A7           = 21; // Unused
static const uint8_t P_MCU_RXI      = 0;
static const uint8_t P_MCU_TXO      = 1;
static const uint8_t P_RTC_WAKE     = 2; // Interrupt from RTC
static const uint8_t P_SM_WAKE      = 3; // Interrupt from interrupting sensor module
static const uint8_t P_SLP_XBEE     = 4; // Output, put XBee module to sleep
static const uint8_t P_SM_VCC_EN    = 5; // Output, enable controlled supply voltage to SM
static const uint8_t P_CTS_XBEE     = 6; // Input, XBee's clear to send pin
static const uint8_t P_SD_CD        = 7; // Input, detect whether SD card is inserted
static const uint8_t P_SD_SS        = 8; // Output, slave select pin for SD card SPI
static const uint8_t P_XBEE_RXI     = 9; // Serial communication RXI pin for Xbee
static const uint8_t P_XBEE_TXO     = 10;// Serial communication TXO pin for Xbee
static const uint8_t P_MOSI         = 11;// SPI MOSI pin
static const uint8_t P_MISO         = 12;// SPI MISO pin
static const uint8_t P_SCK          = 13;// SPI SCK pin

XBee xbee = XBee();
XBeeAddress64 addr64 = XBeeAddress64(UPLINK_SH, UPLINK_SL); // SH + SL Address of receiving XBee
ZBTxRequest zbTx;
ZBTxStatusResponse txStatus = ZBTxStatusResponse();
SoftwareSerial xbeeSoftSerial(P_XBEE_TXO, P_XBEE_RXI);

volatile bool wakeUpInterrupt_flag_RTC = false;
volatile bool wakeUpInterrupt_flag_SM  = false;
volatile bool sd_card_is_initialized   = false;
volatile bool ignore_rtc_interrupt     = false;
volatile bool ignore_sm_interrupt      = false;

bool debug_mode_enabled                = false;
bool debug_mode_has_been_switched_off  = false;
bool low_battery_level_detected        = false;
bool first_time                        = true;


void setup(){
  pinMode(P_L1, OUTPUT);
  pinMode(P_L2, OUTPUT);
  pinMode(P_L3, OUTPUT);
  pinMode(P_SM_VCC_EN, OUTPUT);
  digitalWrite(P_L1, LOW);
  digitalWrite(P_L2, LOW);
  digitalWrite(P_L3, LOW);
  digitalWrite(P_SM_VCC_EN, LOW);
  pinMode(P_DBG_ENABLE, INPUT);
  pinMode(P_SD_CD, INPUT);

  // Initialize RTC 
  Wire.begin();
  setupDS3231(true);
  setDS3231time(01, 49, 23, 7, 01, 01, 10); // DS3231 seconds, minutes, hours, day, date, month, year
  pinMode(P_RTC_WAKE, INPUT_PULLUP); // Pin for Wakeup from RTC
  digitalWrite(P_L1, HIGH); // Indicate RTC initialized ok

  // For sleep mode: Shut down things we do not need:
  ACSR = (1 << ACD); //Disable the analog comparator

  // Sensor Module setup
  setupSerialToSM();
  pinMode(P_SM_WAKE, INPUT_PULLUP);

  // Start XBee
  pinMode(P_CTS_XBEE, INPUT);
  pinMode(P_SLP_XBEE, OUTPUT);
  digitalWrite(P_SLP_XBEE, LOW);
  xbeeSoftSerial.begin(9600);
  xbee.setSerial(xbeeSoftSerial);
  while (digitalRead(P_CTS_XBEE) == 1) {}// Wait until XBee is ready after starting up

  // XBee configuration
  uint8_t SM_value = 0x01;
  setATCommandToValue('S', 'M', &SM_value, sizeof(SM_value));
  sendATCommand('W', 'R');
  sendATCommand('A', 'C');
  // The SM configuration does not work

  uint32_t b0, b1, b2, b3;            // Convert PAN ID from little endian (what arudino uses) to big endian (what XBee wants)
  b0 = (PAN_ID & 0x000000FF) << 24u;
  b1 = (PAN_ID & 0x0000FF00) << 8u;
  b2 = (PAN_ID & 0x00FF0000) >> 8u;
  b3 = (PAN_ID & 0xFF000000) >> 24u;
  uint32_t big_endian_pan_id = b0 | b1 | b2 | b3;

  setATCommandToValue('I', 'D', (uint8_t*)&big_endian_pan_id, sizeof(big_endian_pan_id));
  sendATCommand('W', 'R');
  sendATCommand('A', 'C');

  // Wait until XBee network joined
  uint8_t AI_value = 0xFF;
  while (AI_value != 0x00) {
    // "AI" AT command returns network status. 0x00 means successfully joined.
    AI_value = getSingleByteATCmdValue('A', 'I');
    blink(P_L2, 250);// blinking led indicates trying to join XBee network
  }
  digitalWrite(P_L2, HIGH); // Indicate Xbee network joined ok

  // If debug mode is enable we measure and send more often
  if (digitalRead(P_DBG_ENABLE) == LOW) { // Switch on east side = LOW
    debug_mode_enabled = true;
    SAMPLING_INTERVAL = 1;
    HEARTBEAT_INTERVAL = 1;
  }
  else {
    debug_mode_enabled = false;
    SAMPLING_INTERVAL = C_SAMPLING_INTERVAL;
    HEARTBEAT_INTERVAL = C_HEARTBEAT_INTERVAL;
  }

  digitalWrite(P_L1, LOW);
  digitalWrite(P_L2, LOW);
  digitalWrite(P_L3, LOW);

  // Make first RTC interruppt occur shortly afterwards seconds
  setDS3231time(59, 49, 23, 7, 01, 01, 10); // DS3231 seconds, minutes, hours, day, date, month, year
  // Clear interrupt flag in RTC IC: Clear A2F and A1F bits by setting them to 0
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x0F); // write to register (0Fh)
  Wire.write(0b10001000);
  Wire.endTransmission();

  wakeUpInterrupt_flag_RTC = false;
  wakeUpInterrupt_flag_SM = false;
}

void loop()
{
  // START EXTENDED SETUP
  // This first_time part is not in the setup() because it somehow did not work then.
  if (first_time == true && wakeUpInterrupt_flag_RTC  == true) {
    // The first time the RTC interrupt happens, we are still kind of in the setup phase (not programmed very nicely...)
    // Meaning that only now do we learn whether the sensor module is a periodic one or an interrupting one
    first_time = false;
    wakeup_counter = 0;
    heartbeat_counter = C_HEARTBEAT_INTERVAL;
    
    digitalWrite(P_L1, HIGH); // Indicate start acquiring sensor measurement
    turnOnSensorModule();     // Take sensor module measurement
    delay(100);               // Give some time for SM to start if it was not turned on
    Serial.write(0xBB);       // If it is an interrupting sensor module it will respond to this and if it is a periodic one it will respond with its measurement, which will begin with the FRAMESTART_BYTE

    bool checksum_ok = readSensorModuleData();
    turnOffSensorModule();

    // From ignore_rtc_interrupt we can now tell what kind of sensor module we have
    if (ignore_rtc_interrupt == false) {
      blink(P_L3, 1000);  // LED 3 indicates periodic sensor module
    }
    else {
      blink(P_L2, 1000);  // Sensor module is one which will interrupt the communication module
    }
    digitalWrite(P_L1, LOW);

    // Disable interrupt flag in RTC IC: Clear A2F and A1F bits by setting them to 0
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0x0F);       // write to register (0Fh)
    Wire.write(0b10001000);
    Wire.endTransmission();

    // Make next RTC interruppt occur in 5 seconds afterwards seconds
    setDS3231time(55, 49, 23, 7, 01, 01, 10); // DS3231 seconds, minutes, hours, day, date, month, year
    // Clear interrupt flag in RTC IC: Clear A2F and A1F bits by setting them to 0
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0x0F);       // write to register (0Fh)
    Wire.write(0b10001000);
    Wire.endTransmission();

    wakeUpInterrupt_flag_RTC = false;
  }
  // END OF EXTENDED SETUP

  // Send heartbeat signal
  heartbeat_counter++;
  if (heartbeat_counter > C_HEARTBEAT_INTERVAL){
      heartbeat_counter = 0;
      digitalWrite(P_SLP_XBEE, LOW);          // Wake XBee up
      while (digitalRead(P_CTS_XBEE) == 1) {} // Wait until XBee is ready after waking up
      heartbeat();                            // Send heartbeat message
      digitalWrite(P_SLP_XBEE, HIGH);         // XBee sleep
  }
    
  // The usual RTC interrupt when using a periodic sensor module will trigger this:
  if (wakeUpInterrupt_flag_RTC == true && ignore_rtc_interrupt == false) {
    if (debug_mode_enabled == true) {
      // First check if slide switch still selects debug mode
      if (digitalRead(P_DBG_ENABLE) == HIGH) {
        // Debug mode has been disabled by the user
        debug_mode_enabled               = false;
        debug_mode_has_been_switched_off = true;
        SAMPLING_INTERVAL                = C_SAMPLING_INTERVAL;
        HEARTBEAT_INTERVAL               = C_HEARTBEAT_INTERVAL;
      }
      else {
        digitalWrite(P_L1, HIGH);
      }
    }
    wakeup_counter++;

    // Take measurement if it is time
    if (wakeup_counter >= SAMPLING_INTERVAL) {
      wakeup_counter = 0;
      low_battery_check();

      // Take sensor module measurement
      turnOnSensorModule();
      bool checksum_ok = readSensorModuleData();   // To Do: Do something if checksum is not ok
      turnOffSensorModule();

      if (debug_mode_enabled == true) {
        digitalWrite(P_L2, HIGH);
      }

      // Do XBEE transmit
      digitalWrite(P_SLP_XBEE, LOW);                                    // Wake XBee up
      while (digitalRead(P_CTS_XBEE) == 1) {}                           // Wait until XBee is ready after waking up
      xbee_transmit_data(total_xbee_payload, total_xbee_payload_size);  // Send measurement data
      digitalWrite(P_SLP_XBEE, HIGH);                                   // XBee sleep

      if (debug_mode_enabled == true) {
        digitalWrite(P_L3, HIGH);
      }
    }

    if (debug_mode_enabled == true){
      delay(2000);
      digitalWrite(P_L1, LOW);
      digitalWrite(P_L2, LOW);
      digitalWrite(P_L3, LOW);
      // If we are in debug mode we want an interrupt every 10 seconds instead of every minute.
      setDS3231time(50, 49, 23, 7, 01, 01, 10); // DS3231 seconds, minutes, hours, day, date, month, year
    }

    // Disable interrupt flag in RTC IC: Clear A2F and A1F bits by setting them to 0
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0x0F); // write to register (0Fh)
    Wire.write(0b10001000);
    Wire.endTransmission();

    wakeUpInterrupt_flag_RTC = false;
  }

  // The usual interrupt from the sensor module when using an interrupting sensor module will trigger this:
  if (wakeUpInterrupt_flag_SM == true && ignore_sm_interrupt == false) {
    // transmit at every event
    startSerialToSM();
    low_battery_check();

    if (debug_mode_enabled == true) {
      // First check if slide switch still selects debug mode
      if (digitalRead(P_DBG_ENABLE) == HIGH) {
        debug_mode_enabled               = false;
        debug_mode_has_been_switched_off = true;
      }
      else {
        digitalWrite(P_L1, HIGH);
      }
    }

    turnOnSensorModule();
    startSerialToSM();
    readSensorModuleData();
    turnOffSensorModule();
    
    if (debug_mode_enabled == true) {
      digitalWrite(P_L2, HIGH);
    }

    digitalWrite(P_SLP_XBEE, LOW);                                    // Wake XBee up
    while (digitalRead(P_CTS_XBEE) == 1) {}                           // Wait until XBee is ready after waking up:
    xbee_transmit_data(total_xbee_payload, total_xbee_payload_size);  // Send measurement data to gateway
    digitalWrite(P_SLP_XBEE, HIGH);                                   // Xbee sleep

    if (debug_mode_enabled == true){
      digitalWrite(P_L3, HIGH);
      delay(1000);
      digitalWrite(P_L1, LOW);
      digitalWrite(P_L2, LOW);
      digitalWrite(P_L3, LOW);
    }

    wakeUpInterrupt_flag_SM = false;
  }

  goToSleep();
}


/*****************************
   Sensor Module related functions
*/

void turnOnSensorModule() {
  startSerialToSM();
  delay(3);                       // Small delay necessary somehow
  digitalWrite(P_SM_VCC_EN, HIGH);// Enable voltage to sensor module
}

void turnOffSensorModule() {
  digitalWrite(P_SM_VCC_EN, LOW);// Disable voltage to sensor module
  endSerialToSM();
}

// return: true if checksum ok, false if checksum wrong
bool readSensorModuleData() {
  uint8_t sumForChecksum = 0x00;
  // Wait for frame start byte
  uint8_t readByte = 0x00;
  while (readByte != FRAMESTART_BYTE) {
    while (!Serial.available()) {}
    readByte = Serial.read();

    // The sensor module is an interrupt based one and not one that we use periodically, so return. We will note this and not try to ask it with the RTC interrupt anymore.
    if (readByte == 0xBB) {
      ignore_rtc_interrupt = true;
      return false;
    }
  }

  sumForChecksum += FRAMESTART_BYTE;

  // Next byte indicates type of sensormodule
  while (!Serial.available()) {}
  uint8_t SMtypebyte = Serial.read();
  sumForChecksum += SMtypebyte;

  // Next byte indicates length of data
  while (!Serial.available()) {}
  uint8_t SMsizebyte = Serial.read();
  sumForChecksum += SMsizebyte;

  // If sensor module type has changed, or if it is the first time we read from sensor module. Or if the debug mode was disabled we need to cahnge the size of the memory for the payload too
  if (SMtypebyte != SMtype || debug_mode_has_been_switched_off == true) {
    debug_mode_has_been_switched_off = false;

    if (SMtype != 0 || debug_mode_has_been_switched_off == true) {
      // The sensor module was actually changed or debug mode has been switched off
      free(total_xbee_payload);// Throw out data measured but not yet transmitted of old SM for sake of simplicity
      wakeup_counter = 0;
    }

    sizeofSMdata = SMsizebyte;
    SMtype       = SMtypebyte;

    // Allocate space for accumulating required number of measurements before transmit is done
    total_xbee_payload_size = (xbee_header_bytes + sizeofSMdata *1);
    total_xbee_payload      = (uint8_t*) malloc(total_xbee_payload_size);

    // Fill in the header
    total_xbee_payload[0] = 0x00FF & THIS_CM_ID;
    total_xbee_payload[1] = (0xFF00 & THIS_CM_ID)>>8;
    total_xbee_payload[2] = 1; // How many measurements are in this packet
    total_xbee_payload[3] = SMtype;
  }

  while (Serial.available() < sizeofSMdata + 1) {} // Wait until payload arrived plus checksum byte

  // Read the payload
  for (int k = 0; k < sizeofSMdata; k++) {
    uint8_t inByte = (uint8_t) Serial.read();
    sumForChecksum += inByte;
    (total_xbee_payload + xbee_header_bytes + 0 * SMsizebyte)[k] = inByte; // " xbee_header_bytes + (measurement_counter-1)*SMsizebyte" provides offset, for second, third etc measurements
  }

  // Read checksum byte
  uint8_t checksumbyte = Serial.read();

  // Confirm that checksum is ok
  if (uint8_t (checksumbyte + sumForChecksum) == 0xFF) {
    return true;
  }
  else {
    return false;
  }
}

void setupSerialToSM() {
  pinMode(P_MCU_TXO, OUTPUT);
  digitalWrite(P_MCU_TXO, LOW);
  pinMode(P_MCU_RXI, OUTPUT);
  digitalWrite(P_MCU_RXI, LOW);
}

void startSerialToSM() {
  Serial.begin(9600);
}

void endSerialToSM() {
  // End serial connection to sensor module
  // If Serial.end() is not called, the MCU_TXO pin stays at high voltage level
  Serial.end();

  // Pins low
  pinMode(P_MCU_TXO, OUTPUT);
  pinMode(P_MCU_RXI, OUTPUT);
  digitalWrite(P_MCU_TXO, LOW);
  digitalWrite(P_MCU_RXI, LOW);
}

void serialFlush() {
  while (Serial.available() > 0) {
    char t = Serial.read();
  }
}

/*****************************
   XBee
*/
void xbee_transmit_data(uint8_t* myPayload, uint8_t myPayloadSize) {
  zbTx = ZBTxRequest(addr64, myPayload, myPayloadSize);

  bool message_acked = false;
  while (message_acked == false) {
    message_acked = true; // TODO
    xbee.send(zbTx);

    // after sending a tx request, we expect a status response
    // wait up to half second for the status response
    if (xbee.readPacket(500)) {
      // got a response!
      // should be a znet tx status
      if (xbee.getResponse().getApiId() == ZB_TX_STATUS_RESPONSE) {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
        // get the delivery status, the fifth byte
        if (txStatus.getDeliveryStatus() == SUCCESS) {
          message_acked = true;
        } else {
          // the remote XBee did not receive our packet. is it powered on?
          message_acked = false;
        }
      }
    }
  }
}

int xbee_get_rssi() { 
  int rssi = -getSingleByteATCmdValue('D', 'B');
  return rssi;
}

void setATCommandToValue(uint8_t firstChar, uint8_t secondChar, uint8_t* value, uint8_t valueLength){
  uint8_t at_command_container[2];
  at_command_container[0] = firstChar;
  at_command_container[1] = secondChar;

  AtCommandRequest atRequest = AtCommandRequest(at_command_container, value, valueLength);
  AtCommandResponse atResponse = AtCommandResponse();

  // send the command
  xbee.send(atRequest);

  // wait up to xxx milliseconds for the status response
  if (xbee.readPacket(10000)) {
    // got a response!
    // should be an AT command response
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
      xbee.getResponse().getAtCommandResponse(atResponse);
    }
  }
}

// For commands that don't need a value but are executed like AC, FR, etc

// For commands that don't need a value but are executed like AC, FR, etc
void sendATCommand(uint8_t firstChar, uint8_t secondChar) {
  uint8_t at_command_container[2];
  at_command_container[0] = firstChar;
  at_command_container[1] = secondChar;

  AtCommandRequest atRequest   = AtCommandRequest(at_command_container);
  AtCommandResponse atResponse = AtCommandResponse();
  xbee.send(atRequest); // send the command

  // wait up to __ seconds for the status response
  if (xbee.readPacket(10000)) {
    // got a response!
    // should be an AT command response
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
      xbee.getResponse().getAtCommandResponse(atResponse);
    }
  }
}


uint8_t getSingleByteATCmdValue(uint8_t firstChar, uint8_t secondChar){
  uint8_t at_command_container[2];
  at_command_container[0] = firstChar;
  at_command_container[1] = secondChar;

  AtCommandRequest atRequest   = AtCommandRequest(at_command_container);
  AtCommandResponse atResponse = AtCommandResponse();

  uint8_t responseValue = 0xBA; // Some default value

  // send the command
  xbee.send(atRequest);

  // wait up to __ milliseconds for the status response
  if (xbee.readPacket(10000)) {
    // got a response!
    // should be an AT command response
    if (xbee.getResponse().getApiId() == AT_COMMAND_RESPONSE) {
      xbee.getResponse().getAtCommandResponse(atResponse);

      if (atResponse.isOk()) {
        if (atResponse.getValueLength() > 0) {
          responseValue = atResponse.getValue()[0];
        }
      }
    }
  }
  else {
    // No response
    return 0xFF;
  }
  return responseValue;
}


/*****************************
   Measure Battery Voltage in Volts
*/
float measureVBat(){
  uint16_t sensorValue = analogRead(P_VBAT_MEASURE);
  return ((float)sensorValue * 1000 * 3.3 * 3 / 2 / 1023 / 1000); // ok for 1:2
}


/*****************************
   Functions for sleep mode
*/
// Interrupt from RTC
void pin2_isr(void){
  detachInterrupt(0);
  ADCSRA |= (1 << ADEN); //Enable ADC
  wakeUpInterrupt_flag_RTC = true;
}


// Interrupt from sensor module
void pin3_isr(void){
  detachInterrupt(1);
  ADCSRA |= (1 << ADEN); //Enable ADC
  wakeUpInterrupt_flag_SM = true;
}


void goToSleep(){
  cli(); // Disable interrupts to prevent an interrupt from disturbing this sequence
  digitalWrite(P_SLP_XBEE, HIGH); // Set XXBEe to sleep
  ADCSRA &= ~(1 << ADEN); //Disable ADC
  sleep_enable();
  if (ignore_rtc_interrupt == false)
  {
    attachInterrupt(0, pin2_isr, LOW);
  }
  if (ignore_sm_interrupt == false)
  {
    attachInterrupt(1, pin3_isr, RISING);
  }
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  power_twi_disable(); // Two Wire
  power_spi_disable(); // SPI
  power_usart0_disable();
  power_timer0_disable();
  power_timer1_disable();
  power_timer2_disable();
  sleep_bod_disable();
  sei();
  sleep_cpu();
  
  /* wake up here */
  sleep_disable();
  power_twi_enable(); // Two Wire
  power_spi_enable(); // SPI
  power_usart0_enable();
  power_timer0_enable();
  power_timer1_enable();
  power_timer2_enable();
}


/*****************************
   Functions for DS3231 Real Time Clock.
*/
// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val){
  return ( (val / 10 * 16) + (val % 10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val){
  return ( (val / 16 * 10) + (val % 16) );
}


// This function enables or disables the alarm function
void setupDS3231(bool useAlarm1){
  byte dataToWrite = 0b00011100;
  if (useAlarm1 == true)  {
    dataToWrite |= (1 << 0);
  }

  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0x0E);        // set next input to start at (0Eh) register
  Wire.write(dataToWrite); // set seconds
  Wire.endTransmission();

  // Configure time of alarm
  if (useAlarm1 == true) {
    // We use alarm 1 as once per minute
    Wire.beginTransmission(DS3231_I2C_ADDRESS);
    Wire.write(0x07);       // set next input to start at (0Eh) register
    Wire.write(1);          // A1M1 is 0 and second values is zero
    Wire.write( (1 << 7) ); // A1M2 is 1 and we dont care about minute value we write here since it wont be checked, so we set them to 0
    Wire.write( (1 << 7) ); // A1M3 is 1 and we dont care about hoour value we write here since it wont be checked, so we set them to 0
    Wire.write( (1 << 7) ); // A1M4 is 1 and we dont care about day value we write here since it wont be checked, so we set them to 0
    Wire.endTransmission();
  }
}


void setDS3231time(byte second, byte minute, byte hour, byte dayOfWeek, byte dayOfMonth, byte month, byte year){
  // sets time and date data to DS3231
  Wire.beginTransmission(DS3231_I2C_ADDRESS);
  Wire.write(0);                    // set next input to start at the seconds register
  Wire.write(decToBcd(second));     // set seconds
  Wire.write(decToBcd(minute));     // set minutes
  Wire.write(decToBcd(hour));       // set hours
  Wire.write(decToBcd(dayOfWeek));  // set day of week (1=Sunday, 7=Saturday)
  Wire.write(decToBcd(dayOfMonth)); // set date (1 to 31)
  Wire.write(decToBcd(month));      // set month
  Wire.write(decToBcd(year));       // set year (0 to 99)
  Wire.endTransmission();
}

/*****************************
   Function for blinking debugging LEDs
*/
void blink(int pin, int duration){
  digitalWrite(pin, HIGH);
  delay(duration);
  digitalWrite(pin, LOW);  
  delay(duration);
}

/*****************************
   Function for heartbeat message
*/
void heartbeat(){
  float currentBatteryVoltage = measureVBat(); // Get battery voltage
  float myRSSI = 26;  // Dummy value
  myRSSI = (float) xbee_get_rssi();
  PayloadClass myMessage;
  myMessage.set_id(THIS_CM_ID);
  myMessage.set_SMtype(20);
  myMessage.set_payload(currentBatteryVoltage, myRSSI, 0, 0, 0);
  xbee_transmit_data(myMessage.get_payload_ptr(), sizeof(myMessage.payload));
}

/*****************************
   Function low battery check
*/
void low_battery_check(){
  float currentBatteryVoltage = measureVBat();
  if (low_battery_level_detected == false && currentBatteryVoltage < LOW_BATTERY_WARNING_LEVEL && currentBatteryVoltage > 1) {// > 1 check because if it is supplied through the power jack, then VBat is zero. So I use 1V here, because with 1V at VBat the system would not be running.
    digitalWrite(P_SLP_XBEE, LOW);             // Wake XBee up
    while (digitalRead(P_CTS_XBEE) == 1) {}    // Wait until XBee is ready
    PayloadClass myMessage;
    myMessage.set_id(THIS_CM_ID);
    myMessage.set_SMtype(13);
    myMessage.set_payload(currentBatteryVoltage, 0, 0, 0, 0);
    xbee_transmit_data(myMessage.get_payload_ptr(), sizeof(myMessage.payload));
    digitalWrite(P_SLP_XBEE, HIGH);            // Xbee sleep
    low_battery_level_detected = true;         // So that only the first time will the warning be transmitted
    ignore_rtc_interrupt       = true;         // Do not wake up ever again to safe the battery
    ignore_sm_interrupt        = true;
  }
}
