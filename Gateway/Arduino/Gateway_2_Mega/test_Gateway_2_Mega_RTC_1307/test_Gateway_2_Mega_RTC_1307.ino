/******************************************************************************
DS1307_RTC_Demo.ino
Jim Lindblom @ SparkFun Electronics
original creation date: October 2, 2016
https://github.com/sparkfun/SparkFun_DS1307_RTC_Arduino_Library

Configures, sets, and reads from the DS1307 real-time clock (RTC).

Resources:
Wire.h - Arduino I2C Library

Development environment specifics:
Arduino 1.6.8
SparkFun RedBoard
SparkFun Real Time Clock Module (v14)
******************************************************************************/
#include <SparkFunDS1307RTC.h>
#include <Wire.h>

// Comment out the line below if you want month printed before date.
// E.g. October 31, 2016: 10/31/16 vs. 31/10/16
//#define PRINT_USA_DATE

#define SQW_INPUT_PIN 2   // Input pin to read SQW
#define SQW_OUTPUT_PIN 13 // LED to indicate SQW's state

void setup() 
{
  // Use the serial monitor to view time/date output
  Serial.begin(9600);
  Serial.print("Compiler date: ");
  Serial.print(__TIME__);
  Serial.print(", ");
  Serial.println(__DATE__);  
  Serial.println("");
  pinMode(SQW_INPUT_PIN, INPUT_PULLUP);
  pinMode(SQW_OUTPUT_PIN, OUTPUT);
  digitalWrite(SQW_OUTPUT_PIN, digitalRead(SQW_INPUT_PIN));
  
  rtc.begin(); // Call rtc.begin() to initialize the library


  // Now set the time...
  // You can use the autoTime() function to set the RTC's clock and
  // date to the compiliers predefined time. (It'll be a few seconds
  // behind, but close!)
  //rtc.autoTime(); // Did not work on 29.08.2018 18:30
                  //                            19:30
                  //                            21:04
                  //                 30.08.2018 23:50
                  // Did work on     31.08.2018 14:20
                  // Did work partly 11.01.2018 17:32 (date worked, time didn't)
  // Or you can use the rtc.setTime(s, m, h, day, date, month, year)
  // function to explicitly set the time:
  // e.g. 7:32:16 | Monday October 31, 2016:
  rtc.setTime(00, 39, 17, 6, 11, 01, 19);  // Uncomment to manually set time (does work)
  rtc.set24Hour(); // Use rtc.set24Hour to set to 24-hour mode
  // rtc.set12Hour(); // Use rtc.set12Hour to set to 12-hour mode
}

void loop() 
{
  static int8_t lastSecond = -1;
  
  // Call rtc.update() to update all rtc.seconds(), rtc.minutes(),
  // etc. return functions.
  rtc.update();

  if (rtc.second() != lastSecond) // If the second has changed
  {
    printTime(); // Print the new time
    
    lastSecond = rtc.second(); // Update lastSecond value
  }

  // Read the state of the SQW pin and show it on the
  // pin 13 LED. (It should blink at 1Hz.)
  digitalWrite(SQW_OUTPUT_PIN, digitalRead(SQW_INPUT_PIN));
}

void printTime()
{
  Serial.print(String(rtc.hour()) + ":"); // Print hour
  if (rtc.minute() < 10)
    Serial.print('0'); // Print leading '0' for minute
  Serial.print(String(rtc.minute()) + ":"); // Print minute
  if (rtc.second() < 10)
    Serial.print('0'); // Print leading '0' for second
  Serial.print(String(rtc.second())); // Print second

  if (rtc.is12Hour()) // If we're in 12-hour mode
  {
    // Use rtc.pm() to read the AM/PM state of the hour
    if (rtc.pm()) Serial.print(" PM"); // Returns true if PM
    else Serial.print(" AM");
  }
  
  Serial.print(" | ");

  // Few options for printing the day, pick one:
  Serial.print(rtc.dayStr()); // Print day string
  //Serial.print(rtc.dayC()); // Print day character
  //Serial.print(rtc.day()); // Print day integer (1-7, Sun-Sat)
  Serial.print(" - ");
#ifdef PRINT_USA_DATE
  Serial.print(String(rtc.month()) + "/" +   // Print month
                 String(rtc.date()) + "/");  // Print date
#else
  Serial.print(String(rtc.date()) + "/" +    // (or) print date
                 String(rtc.month()) + "/"); // Print month
#endif
  Serial.println(String(rtc.year()));        // Print year
}
