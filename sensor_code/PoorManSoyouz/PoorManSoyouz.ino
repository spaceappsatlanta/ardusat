/*
  PoorManSoyouz ArduSat sketch
  
  This is the base TempSensorlate to deploy the PoorManSoyouz's arduino module:
  You can assemble your sensor circuit and fill in minimal parameters to
  collect data via the PoorManSoyouz WebService.
  
  Arduino data is written to the webservice via the Raspberry Pi client firmware.
  It is uniquely identified by the raspberry pi client.
  
  Possible alternatives include attaching an xBee linked to a PC if an adequate
  webservice client is created.
  
 */

// CAUTION: you may not wire sensors to pins 0 and 1, which are used by 
// ArduSat for Rx/Tx with the platform bus

#include <FreeSixIMU.h>
#include <FIMU_ADXL345.h>
#include <FIMU_ITG3200.h>

#define DEBUG
#ifdef DEBUG
#include "DebugUtils.h"
#endif

#include "CommunicationUtils.h" // for inertial sensor
#include "FreeSixIMU.h" // inertial sensor
#include <Wire.h> // for inertial sensor and temp sensor
#include "TSL2561.h" // for temp sensor and 

void setup() {
  Serial.begin(115200);
  Wire.begin();

  delay(5);
  my3IMU.init();
  delay(5);
}

// rate at which the arduino should sample data
int SampleInterval = 100; //ms

// Enable your sensors and specify pins below, starting at digital 2 and analog A0  
bit enableTempSensor = 0;
int TempSensorPin = A0;

bit enableLightSensor = 0;
int lightSensorPin = A1;

bit enableCam = 0;
int camControlPin = 12;
int camDataPin = 13;

bit enableIMU = 0;
int imuClockPin = A5;
int imuDataPin = A4;

bit enableMag = 0;
int magClockPin = A3
int magDataPin = A2

bit enableIrSensor = 0;
int irSensorPin = 11;

//variables to store the value coming from the analog sensors
int tempSensorCelsius = 0; 
int irSensorValue = 0; 
int lightSensorValue = 0; 

//variables to store the 3D values
float rawImuValues[6]; // coming from the inertial sensor
float rawMagValues[3]; // coming from magnetometer sensor

// Initialize the FreeIMU object for the inertial sensor
FreeSixIMU my3IMU = FreeSixIMU();

if(enableTempSensor){
  // initialize temp sensor
  int i2cTemperatureAddress = 0x48;
  TSL2561 tsl(TSL2561_ADDR_FLOAT); // note below
  // The address will be different depending on whether you let
  // the ADDR pin float (addr 0x39), or tie it to ground or vcc. In those cases
  // use TSL2561_ADDR_LOW (0x29) or TSL2561_ADDR_HIGH (0x49) respectively
  if (tsl.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("No sensor?");
    while (1);
  }
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //tsl.setGain(TSL2561_GAIN_0X);         // set no gain (for bright situtations)
  tsl.setGain(TSL2561_GAIN_16X);      // set 16x gain (for dim situations)
  
  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situtations!
  tsl.setTiming(TSL2561_INTEGRATIONTIME_13MS);  // shortest integration time (bright light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_101MS);  // medium integration time (medium light)
  //tsl.setTiming(TSL2561_INTEGRATIONTIME_402MS);  // longest integration time (dim light)
  
  // Now we're ready to get readings!
  
  Wire.begin();
}

void setup() {
  // declare the ledPin as an OUTPUT:
  pinMode(ledPin, OUTPUT);  
}

/*
  Sensor reading loop for data sampling
*/

void loop() { 
  // read the value from the 1 dimensional sensors:
  if(enableTempSensor){
    // temperature sensor 
    float tempSensorCelsius = getTempSensorCelsius();
    //Serial.print("Celsius: ");
    //Serial.println(celsius);
    //float fahrenheit = (1.8 * celsius) + 32;  
    //Serial.print("Fahrenheit: ");
    //Serial.println(fahrenheit);
  }
  
  if(enableLightSensor){
    lightsSensorValue = analogRead(lightSensorPin);
  }
  
  if(enableIrSensor){
    // read the infrared, fullspecrtrum diode or 'visible' (difference between the two) channels.
    
    // SLOW method / multiple calls
    // This can take 13-402 milliseconds! Uncomment whichever of the following you want to read
    //uint16_t x = tsl.getLuminosity(TSL2561_VISIBLE);     
    //uint16_t x = tsl.getLuminosity(TSL2561_FULLSPECTRUM);
    //uint16_t x = tsl.getLuminosity(TSL2561_INFRARED);
    //Serial.println(x, DEC);
  
    // FAST method, single call
    // More advanced data read example. Read 32 bits with top 16 bits IR, bottom 16 bits full spectrum
    // That way you can do whatever math and comparisons you want!
    uint32_t irSensorValueLum = tsl.getFullLuminosity();
    uint16_t irSensorValueIR, irSensorValueFull;
    irSensorValueIR = irSensorValueLum >> 16;
    irSensorValueFull = irSensorValueLum & 0xFFFF;
    irSensorValueVisible = irSensorValueFull-irSensorValueIR;
    irSensorValueLux = tsl.calculateLux(irSensorValueFull, irSensorValueIR);
    //Serial.print("IR: "); Serial.print(irSensorValueIR);   Serial.print("\t\t");
    //Serial.print("Full: "); Serial.print(irSensorValueFull);   Serial.print("\t");
    //Serial.print("Visible: "); Serial.print(irSensorValueFull - irSensorValueIR);   Serial.print("\t");
    //Serial.print("Lux: "); Serial.println(irSensorValueLux);
  }
  
  // read the value from the 3D sensors:
  
  if(enableIMU){
    // read the 3D value from the inertial sensor:  
    my3IMU.getRawValues(raw);
    serialPrintFloatArr(rawImuValues, 6);
    Serial.println(""); //line break
  }
  
  if(enableMag){
    // TODO read magnetometer data into rawMagValues[3];
  }

  // read the value from the camera:
  
  if(enableCam){
    // TODO 
    
  }
  
  // Next sample iteration
  delay(SampleInterval);
}


float getTempSensorCelsius(){
  // I2C read from temperature
  Wire.requestFrom(i2cTemperatureAddress,2); 

  byte MSB = Wire.read();
  byte LSB = Wire.read();

  //it's a 12bit int, using two's compliment for negative
  int TemperatureSum = ((MSB << 8) | LSB) >> 4; 

  float celsius = TemperatureSum*0.0625;
  return celsius;
}
