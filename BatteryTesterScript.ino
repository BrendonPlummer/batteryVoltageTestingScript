/*
Battery Testing Script
======================
**author  BrendonP
**company Excelerate Ltd.

Program detects when voltage of circuit exceeds final discharge voltage (as conf) and starts timer.
Once fully depleted again, stops program and time.

Edit constant values to match needs.
Use putty serial, set up logging functionality, saves all serial output to txt file. 

TODO: Remove full date and just do time elapsed - clock_t
*/
#include <stdio.h>
#include "Timer.h"

Timer         timer;
float         value;
float         voltage;
float         convertedValueDisplay;
float         convertedBatteryVoltage;
float         firstRead;
float         lastRead;
float         checkValue;
float         checkConvValue;
float         checkBattVolt;
bool          hasCharge;
bool          startUp                 = false;
const float   finalVoltageAtDischarge = 8.25;        // Look for battery data sheet and find final voltage
const float   Vin                     = 12.58;       // Check values for each battery used, use multimeter and input values here.
const float   Vout                    = 2.51;        // Check values for each battery used, use multimeter and input values here.
const float   batteryConversionValue  = Vin / Vout;
const float   conversionValue         = 3.3 / 1024;  // Maximum voltage of microcontroller / ADC resolution (inputRead funciton has 1024 resolution max)
const uint8_t analogPin               = A2;          // Input pin of microcontroller

void checkSerial() {
  checkValue = analogRead(analogPin);
  checkConvValue = checkValue * conversionValue;
  checkBattVolt = checkConvValue * batteryConversionValue;

  (checkBattVolt > finalVoltageAtDischarge) ? hasCharge = true : hasCharge = false;
  Serial.println("Checking for power input...");
}

void checkBattery(){
  (hasCharge) ? timer.start() : timer.stop();
  while (hasCharge) {
    uint8_t totalSeconds = (timer.read() / 1000) % 60;
    uint8_t totalMinutes = ((timer.read() / 1000) / 60) % 60;
    uint8_t totalHours   = ((timer.read() / 1000) / 60) / 60;
    Serial.printf("TIME:\t\t\t   %02d:%02d:%02d\n", totalHours, totalMinutes, totalSeconds);

    value = analogRead(analogPin);
    Serial.print("First reading value:       ");
    Serial.println(value);

    convertedValueDisplay = value * conversionValue;
    Serial.print("Converted value:           ");
    Serial.println(convertedValueDisplay);

    convertedBatteryVoltage = convertedValueDisplay * batteryConversionValue;  // Need to see if consistent across all batteries
    Serial.print("Converted Battery Voltage: ");
    Serial.println(convertedBatteryVoltage);
    Serial.println();

    (startUp) ? firstRead = convertedBatteryVoltage : startUp = false;
    (convertedBatteryVoltage <= finalVoltageAtDischarge) ? hasCharge = false : lastRead = convertedBatteryVoltage;
    startUp = false;
    delay(5000);
  }

  Serial.printf("Battery test is now finished. \nFirst read: %f, Final read: %f", firstRead, lastRead);
  Serial.println("\n-------------Battery dead-------------");
}

void setup() {
  Serial.begin();
  startUp = true;
  hasCharge = true;
}

void loop() {
  delay(4000);
  checkSerial();
  checkBattery();
}