/*
  Functiontest_0x12_U-Value_100_meas.ino - Advanced functionality sketch for the U-Value board.
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
#define ADC_INTERRUPT 3
#define TCOUP_CS 15
#define DEBUG_LED 10
#define DSP_BUS_1 17
#define DSP_BUS_2 16

OneWire _DSP_BUS_1(DSP_BUS_1);
OneWire _DSP_BUS_2(DSP_BUS_2);

SoftwareSerial SoftSerial(6, 7); //RX TX
DallasTemperature dsp_sens_1(&_DSP_BUS_1);
DallasTemperature dsp_sens_2(&_DSP_BUS_2);
Adafruit_MAX31855 tcouple(TCOUP_CS);
MCP3911 mcp3911;

volatile uint8_t index = 0; //Array-Index
long values_ch0[100] = {};  //Array for the 100 measurements

//ADC interrupt function
void ch0_data_interrupt(void)
{
  values_ch0[index] = mcp3911.read_raw_data(REG_CHANNEL0);
  index++;
  if (index > 99)
    mcp3911.enter_reset_mode(); //Enter reset mode to stop any more interrupts
}

//ADC calibration function
void ch0_calibrate_offset(void)
{
  mcp3911.write_offset(0, REG_OFFCAL_CH0);  //Write initial offset to zero

  digitalWrite(ADC_IN_ENABLE, LOW);         //Physically ground the input of the uV-Sensor
  digitalWrite(ADC_IN_SHORT, HIGH);
  delay(100);

  //Start 100 measurements with interrupts
  index = 0;
  mcp3911.exit_reset_mode();    //Exit reset mode to start interrupts again.
  while (index < 100);          //Do nothing until 100 measurements are completed

  digitalWrite(ADC_IN_SHORT, LOW);          //Enable connection from Sensor to ADC again
  digitalWrite(ADC_IN_ENABLE, HIGH);

  long average = 0;             //Calculate average of the 100 measurements
  for (int i = 0; i < 100; i++) {
    average += values_ch0[i];
  }
  long offset = (average / 100) * -1; //We multiply by -1 since the offset gets added,
  //but to be correct we want it subtracted

  mcp3911.write_offset(offset, REG_OFFCAL_CH0);   //Write offset to register

  float offset_volt = mcp3911.data_to_voltage((average / 100), REG_CHANNEL0);

  Serial.print("Offset CH0 = ");
  Serial.print(offset_volt, 8);
  Serial.println(" V");

  index = 0;

  delay(100);
}

void setup() {
  pinMode(ADC_IN_ENABLE, OUTPUT);
  pinMode(ADC_IN_SHORT, OUTPUT);
  pinMode(DEBUG_LED, OUTPUT);

  digitalWrite(ADC_IN_ENABLE, HIGH);  //Enable physical connection between ADC and Sensor.
  digitalWrite(ADC_IN_SHORT, LOW);
  digitalWrite(DEBUG_LED, LOW);

  Serial.begin(9600);
  SoftSerial.begin(9600);

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
  settings.EN_OFFCAL = 0b1;        //Digital offset calibration on both channels enabled
  settings.EN_GAINCAL = 0b0;       //Gain calibration on both channels disabled
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
  mcp3911.enter_reset_mode();      //Enter reset mode to prevent any measurements from being taken

  attachInterrupt(digitalPinToInterrupt(ADC_INTERRUPT), ch0_data_interrupt, FALLING);  //Call "ch0_data_interrupt" whenever an edge on the DR-Pin occurs

  delay(100);                      //Add a delay until the interrupt routine has started

  ch0_calibrate_offset();          //Initially calibrate MCP3911
  mcp3911.exit_reset_mode();       //Exit reset mode to start interrupts again.
}

void loop() {
  //If 100 measurement interrupts have taken place
  //calculate the average of all the measurements that were taken into "values_ch0[]".
  if (index > 99) {
    digitalWrite(DEBUG_LED, LOW);

    dsp_sens_1.requestTemperatures();
    dsp_sens_2.requestTemperatures();

    double average = 0;
    for (int i = 0; i < 100; i++) {
      average += values_ch0[i];
    }

    float voltage_ch0 = mcp3911.data_to_voltage(average / 100, REG_CHANNEL0); //Data to voltage needs the gain setting of the chosen channel

    Serial.print("Voltage CH0 = ");
    Serial.print(voltage_ch0, 8);
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

    SoftSerial.print("Test");

    index = 0;

    digitalWrite(DEBUG_LED, HIGH);
    ch0_calibrate_offset();           //Calibrate MCP3911 for next measurement
    mcp3911.exit_reset_mode();        //Exit reset mode to start next measurement cycle
  }
}
