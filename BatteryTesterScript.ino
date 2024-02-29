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
time_t        myTime;
const float   finalVoltageAtDischarge = 8.25;        // Look for battery data sheet and find final voltage
const float   Vin                     = 12.58;       // Change as needed
const float   Vout                    = 2.54;        // Change as needed
const float   conversionValue         = 3.3 / 1024;  // Maximum voltage of microcontroller / ADC resolution (inputRead funciton has 1024 resolution max)
const uint8_t analogPin               = A2;          // Input pin of microcontroller
const float   batteryConversionValue  = Vin / Vout;
bool          startUp                 = false;


void setup() {
  Serial.begin();
  startUp = true;
  hasCharge = true;
  myTime = time(NULL);
}

void checkSerial() {
  float checkValue = analogRead(analogPin);
  float checkConvValue = checkValue * conversionValue;
  float checkBattVolt = checkConvValue * batteryConversionValue;

  (checkBattVolt > finalVoltageAtDischarge) ? hasCharge = true : hasCharge = false;
}

void loop() {
  checkSerial();
  if (hasCharge) {
    Serial.println("-------------Starting battery test-------------");
    while (hasCharge) {
      value = analogRead(analogPin);
      myTime = time(NULL);
      Serial.println(ctime(&myTime));
      Serial.print("First reading value: ");
      Serial.println(value);

      convertedValueDisplay = value * conversionValue;
      Serial.print("Converted value: ");
      Serial.println(convertedValueDisplay);

      convertedBatteryVoltage = convertedValueDisplay * batteryConversionValue;  // Need to see if consistent across all batteries
      Serial.print("Converted Battery Voltage: ");
      Serial.println(convertedBatteryVoltage);


      (startUp) ? firstRead = convertedBatteryVoltage : startUp = false;
      (convertedBatteryVoltage <= finalVoltageAtDischarge) ? hasCharge = false : lastRead = convertedBatteryVoltage;
      startUp = false;
      delay(1000);
    }
    Serial.println("-------------Battery has now died-------------");
    Serial.printf("First read = %s, Final Read = %s", firstRead, lastRead);
  }
}