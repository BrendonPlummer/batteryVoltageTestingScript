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
#include <time.h>

float         value;
float         voltage;
float         convertedValueDisplay;
float         convertedBatteryVoltage;
float         firstRead;
float         lastRead;
bool          hasCharge;
// static time_t myTime;
uint8_t       displayTimeHour;
uint8_t       displayTimeMinute;
uint8_t       displayTimeSecond;
const float   finalVoltageAtDischarge = 8.25;        // Look for battery data sheet and find final voltage
const float   Vin                     = 12.58;       // Change as needed
const float   Vout                    = 2.51;        // Change as needed07
const float   conversionValue         = 3.3 / 1024;  // Maximum voltage of microcontroller / ADC resolution (inputRead funciton has 1024 resolution max)
const uint8_t analogPin               = A2;          // Input pin of microcontroller
const float   batteryConversionValue  = Vin / Vout;
bool          startUp                 = false;

float checkValue;
float checkConvValue;
float checkBattVolt;

void checkSerial() {
  checkValue = analogRead(analogPin);
  checkConvValue = checkValue * conversionValue;
  checkBattVolt = checkConvValue * batteryConversionValue;

  (checkBattVolt > finalVoltageAtDischarge) ? hasCharge = true : hasCharge = false;
  // if (hasCharge) {
  //   myTime = time(0);
  // }
  Serial.println("CheckSerial Reached");
}

void checkBattery(){
  // Reset timer for battery function
  static time_t myTime;
  myTime            = time(NULL);
  displayTimeHour   = 0;
  displayTimeMinute = 0;
  displayTimeSecond = 0;

  while (hasCharge) {
    value = analogRead(analogPin);
    myTime = time(NULL);
    displayTimeHour   = (myTime / 3600) % 24;
    displayTimeMinute = (myTime / 60) % 60;
    displayTimeSecond = myTime % 60;
    Serial.printf("Time: %02d:%02d:%02d\n", displayTimeHour, displayTimeMinute, displayTimeSecond);
    Serial.print("First reading value: ");
    Serial.println(value);

    convertedValueDisplay = value * conversionValue;
    Serial.print("Converted value: ");
    Serial.println(convertedValueDisplay);

    convertedBatteryVoltage = convertedValueDisplay * batteryConversionValue;  // Need to see if consistent across all batteries
    Serial.print("Converted Battery Voltage: ");
    Serial.println(convertedBatteryVoltage);
    Serial.println();

    (startUp) ? firstRead = convertedBatteryVoltage : startUp = false;
    (convertedBatteryVoltage <= finalVoltageAtDischarge) ? hasCharge = false : lastRead = convertedBatteryVoltage;
    startUp = false;
    delay(1000);
  }
  Serial.println("-------------Battery dead-------------");
}

void setup() {
  Serial.begin();
  startUp = true;
  hasCharge = true;
}

void loop() {
  checkSerial();
  checkBattery();
}