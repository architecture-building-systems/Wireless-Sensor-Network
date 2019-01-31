/*
  Functiontest_0x12_U-Value_simple.ino - Basic functionality sketch for the U-Value Sensorboard
  Version 1.0
  Created by Ruben J. Stadler, May 10, 2018.
  <rstadler@ethz.ch>
  
  This sketch is licensed under the GPL 3.0 license.
*/

#include <SPI.h>
#include <arduino.h>
#include <MCP3911.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>
#include <Adafruit_MAX31855.h>


#define ADC_CLK 9       //Pin 9 is the OC1A-Pin of the Arduino Pro Mini and goes to OSC1-Pin of the MCP3911
#define ADC_CS 8        //Pin 8 of the Arduino Pro Mini goes to the CS-Pin of the MCP3911
#define ADC_IN_ENABLE 2
#define ADC_IN_SHORT 4
#define TCOUP_CS 15
#define DEBUG_LED 10
#define DSP_BUS_1 17
#define DSP_BUS_2 16

OneWire _DSP_BUS_1(DSP_BUS_1);
OneWire _DSP_BUS_2(DSP_BUS_2);

SoftwareSerial SoftSerial(6,7); //RX TX 
DallasTemperature dsp_sens_1(&_DSP_BUS_1);
DallasTemperature dsp_sens_2(&_DSP_BUS_2);
Adafruit_MAX31855 tcouple(TCOUP_CS);
MCP3911 mcp3911;

void setup() {
  Serial.begin(9600);
  SoftSerial.begin(9600);
  
  pinMode(ADC_IN_ENABLE, OUTPUT);
  pinMode(ADC_IN_SHORT, OUTPUT);
  pinMode(DEBUG_LED, OUTPUT);

  digitalWrite(ADC_IN_ENABLE, HIGH);
  digitalWrite(ADC_IN_SHORT, LOW);
  digitalWrite(DEBUG_LED, LOW);

  dsp_sens_1.begin();
  dsp_sens_1.setResolution(12);

  dsp_sens_2.begin();
  dsp_sens_2.setResolution(12);
  
  mcp3911.begin(ADC_CLK, ADC_CS);     //Initialize MCP3911
  mcp3911.generate_CLK();               //Generate 4MHZ clock on CLOCK_PIN

  REGISTER_SETTINGS settings = {};
  //PHASE-SETTINGS
  settings.PHASE    = 0;           //Phase shift between CH0/CH1 is 0
  //GAIN-SETTINGS
  settings.BOOST     = 0b10;       //Current boost is 1 
  settings.PGA_CH1   = 0b000;      //CH1 gain is 1
  settings.PGA_CH0   = 0b000;      //CH0 gain is 1
  //STATUSCOM-SETTINGS
  settings.MODOUT    = 0b00;       //No modulator output enabled
  settings.DR_HIZ    = 0b1;        //DR pin state is logic high when data is not ready
  settings.DRMODE    = 0b00;       //Data ready pulses from lagging ADC are output on DR-Pin
  settings.READ      = 0b10;       //Adress counter loops register types
  settings.WRITE     = 0b1;        //Adress counter loops entire register map
  settings.WIDTH     = 0b11;       //CH0 and CH1 are in 24bit-mode
  settings.EN_OFFCAL = 0b0;        //Digital offset calibration on both channels disabled
  settings.EN_GAINCAL = 0b0;       //Group delay on both channels disabled
  //CONFIG-SETTINGS
  settings.PRE       = 0b00;       //AMCLK = MCLK
  settings.OSR       = 0b011;      //Oversamplingratio is set to 256
  settings.DITHER    = 0b11;       //Dithering on both channels maximal
  settings.AZ_FREQ   = 0b0;        //Auto-zeroing running at lower speed
  settings.RESET     = 0b00;       //Neither ADC in Reset mode
  settings.SHUTDOWN  = 0b00;       //Neither ADC in Shutdown
  settings.VREFEXT   = 0b0;        //Internal voltage reference enabled
  settings.CLKEXT    = 0b1;        //External clock drive on OSC1-Pin enabled

  mcp3911.configure(settings);     //Configure the MCP3911 with the settings above
  delay(100);
}

void loop() {
  digitalWrite(DEBUG_LED, LOW);
  
  float value0 = mcp3911.read_chX(REG_CHANNEL0); //Read value of CH0
  
  dsp_sens_1.requestTemperatures();
  dsp_sens_2.requestTemperatures();
 
  Serial.print("Voltage CH0 = ");       //Print value of CH0
  Serial.print(value0, 8);
  Serial.println(" V");

  Serial.print("Thermocouple: ");
  Serial.print(tcouple.readCelsius());
  Serial.println(" °C");
  Serial.print("Temperature Sensor 1: ");
  Serial.print(dsp_sens_1.getTempCByIndex(0));
  Serial.println(" °C");
  Serial.print("Temperature Sensor 2: ");
  Serial.print(dsp_sens_2.getTempCByIndex(0));
  Serial.println(" °C");
  Serial.println("");

  SoftSerial.println("Test");

  //delay(500);
  digitalWrite(DEBUG_LED, HIGH);
  delay(100);
}
