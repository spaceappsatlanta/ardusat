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
#include <i2cmaster.h>

#define DEBUG
#ifdef DEBUG
#include "DebugUtils.h"
#endif

#include "CommunicationUtils.h" // for inertial sensor
#include "FreeSixIMU.h" // inertial sensor
#include "TSL2561.h" // for temp sensor and 
#include <Wire.h> // for inertial sensor and temp sensor


// rate at which the arduino should sample data
int SampleInterval = 100; //ms

enum sensorID {
  idUnknown,
  idLuminosity,
  idTemperatureDigital,
  idTemperatureIR,
  idInertial,
  idMagnetometer,  
  idCamera
};

// Enable your sensors and specify pins below, starting at digital 2 and analog A0  
boolean enableTempSensor = true;
int i2cTemperatureAddress = 0x48;

boolean enableTempIrSensor = true;

boolean enableCam = false;
int camControlPin = 12;
int camDataPin = 13;

boolean enableImu = true;
int imuClockPin = A1;
int imuDataPin = A0;

boolean enableMagSensor = true;
int MAG_ADDR = 0x0E;

boolean enableLumSensor = true;
int lumSensorPin=0x39; // note below
// The address will be different depending on whether you let
// the ADDR pin float (addr 0x39), or tie it to ground or vcc. In those cases
// use TSL2561_ADDR_LOW (0x29) or TSL2561_ADDR_HIGH (0x49) respectively

//variables to store the value coming from the analog sensors
int tempSensorCelsius = 0; 
int lumSensorValueIR = 0; 
int lumSensorValueVisible = 0; 
int lumSensorValueFull = 0; 
int lumSensorValueLux = 0; 

//variables to store the 3D values
float rawImuValues[6]; // coming from the inertial sensor
float rawMagValues[3]; // coming from magnetometer sensor

// Initialize the FreeIMU object for the inertial sensor
FreeSixIMU my3IMU = FreeSixIMU();

// Initialize the lum sensor object for the sensor
TSL2561 tsl(lumSensorPin); 

void setup() {
  Serial.begin(115200);
  Wire.begin();
  if(enableMagSensor){
    MagConfig();
  }

  if(enableLumSensor){
    LumConfig();
  }  
  
  if(enableTempIrSensor){
    TempIrConfig();
  }

  if(enableImu){
    delay(5);
    my3IMU.init();
    delay(5);
  }
}

void LumConfig(void){
  Serial.print("Setting up luminosity sensor...");
  // initialize luminosity sensor
  if (tsl.begin()) {
    Serial.print("Found sensor...");
  } else {
    Serial.print("No sensor?");
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

  Wire.begin();
  Serial.println("complete.");
}

void TempIrConfig(void){
  Serial.print("Setting up IR Temp...");
  i2c_init(); //Initialise the i2c bus
  PORTC = (1 << PORTC2) | (1 << PORTC3);//enable pullups
  Serial.println("complete.");
}

void MagConfig(void){
  Serial.print("Setting up Magnetometer...");
  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x11);              // cntrl register2
  Wire.write(0x80);              // write 0x80, enable auto resets
  Wire.endTransmission();       // stop transmitting

  delay(15);

  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x10);              // cntrl register1
  Wire.write(1);                 // write 0x01, active mode
  Wire.endTransmission();       // stop transmitting
  Serial.println("complete.");
}

/*
  Sensor reading loop for data sampling
 */

void loop() { 
  // read the value from the 1 dimensional sensors:
  if(enableTempSensor){
    // digital temperature sensor 
    float tempSensorCelsius = getTempSensorCelsius();
    Serial.print("Digital thermometer Celsius: ");
    Serial.println(tempSensorCelsius);
    float fahrenheit = (1.8 * tempSensorCelsius) + 32;  
    Serial.print("Digital thermometer Fahrenheit: ");
    Serial.println(fahrenheit);
    Serial.println();
    //prepare frame (Sid, L, databytes)
    byte * tempData;
    tempData=(byte*)&tempSensorCelsius;
    // data segment: 1 float value
    outputFrame(idTemperatureDigital,4,tempData);
  }

  if(enableLumSensor){
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
    uint32_t lumSensorValueLum = tsl.getFullLuminosity();
    uint16_t lumSensorValueVisible, lumSensorValueIR, lumSensorValueFull, lumSensorValueLux;
    lumSensorValueIR = lumSensorValueLum >> 16;
    lumSensorValueFull = lumSensorValueLum & 0xFFFF;
    lumSensorValueVisible = lumSensorValueFull-lumSensorValueIR;
    lumSensorValueLux = tsl.calculateLux(lumSensorValueFull, lumSensorValueIR);
    Serial.print("Lum Sensor IR: "); Serial.print(lumSensorValueIR);   Serial.println();
    Serial.print("Lum Sensor Full: "); Serial.print(lumSensorValueFull);   Serial.println();
    Serial.print("Lum Sensor Visible: "); Serial.print(lumSensorValueVisible);   Serial.println();
    Serial.print("Lum Sensor Lux: "); Serial.println(lumSensorValueLux);
    Serial.println();
    //outputFrame(Sid, L, databytes)
    byte lumData[10];
    // data segment: 4 int16 values: IR, Full, Visible, Lux
    lumData[0]=lumSensorValueIR >>8;
    lumData[1]=lumSensorValueIR && 0xFF;
    lumData[2]=lumSensorValueFull >>8;
    lumData[3]=lumSensorValueFull && 0xFF;
    lumData[4]=lumSensorValueVisible >>8;
    lumData[5]=lumSensorValueVisible && 0xFF;
    lumData[6]=lumSensorValueLux >>8;
    lumData[7]=lumSensorValueLux && 0xFF;
    outputFrame(idLuminosity,8,lumData);
  }

  // read the value from the 3D sensors:

  if(enableImu){
    // read the 3D value from the inertial sensor:  
    my3IMU.getRawValues(rawImuValues);
    //serialPrintFloatArr(rawImuValues, 6);
    for(int i=0;i<6;i++){
      Serial.print("Inertial Axis ");
      Serial.print(i);
      Serial.print(" :");
      Serial.println(rawImuValues[i]);
      
    }
    Serial.println(""); //line break
    byte * imuData;
    imuData=(byte*)&rawImuValues;
    // data segment: 6 floats to byte stream: Ax,Ay,Az,Gx,Gy,Gz
    outputFrame(idInertial,24,imuData);
  }

  if(enableMagSensor){
    // data segment: Mx,My,Mz,Heading (wrtZ)
    byte * magData;
    float pi = 3.141597;
    magData[0] = ReadMagX();
    magData[2] = ReadMagY();
    magData[4] = ReadMagZ();
    float heading = 0;
    int x=magData[0];
    int y=magData[2];
    int z=magData[4];

    if(y > 0){ 
      heading = 270 - atan2(x,y)*180 / pi; 
    }
    else if( y < 0 ) {
      heading = 90 - atan2(x,y)*180 / pi;
    } 
    if (y == 0 && x < 0) {
      heading = 180.0; 
    }
    if (y == 0 && x > 0) {
      heading = 0.0; 
    }
    magData[6] = heading;
    Serial.print("Magnetometer: X(");
    Serial.print(x);
    Serial.print(") Y(");
    Serial.print(y);
    Serial.print(") Z(");
    Serial.print(z);
    Serial.print(") heading(");
    Serial.print(heading);
    Serial.println(")");
    Serial.println();
    outputFrame(idMagnetometer,10,magData); 
  }

  // read from temperature IR sensor
  if(enableTempIrSensor){
    int dev = 0x00;
    int data_low = 0;
    int data_high = 0;
    int pec = 0;
    
    i2c_start_wait(dev+I2C_WRITE);
    i2c_write(0x07);
    
    //Serial.print("IR temp reading...");
    // read
    i2c_rep_start(dev+I2C_READ);
    data_low = i2c_readAck(); //Read 1 byte and then send ack
    data_high = i2c_readAck(); //Read 1 byte and then send ack
    pec = i2c_readNak();
    i2c_stop();
    //Serial.println("read complete.");
    
    //This converts high and low bytes together and processes temperature, MSB is a error bit and is ignored for temps
    double tempFactor = 0.02; // 0.02 degrees per LSB (measurement resolution of the MLX90614)
    double tempData = 0x0000; // zero out the data
    int frac; // data past the decimal point
    
    // This masks off the error bit of the high byte, then moves it left 8 bits and adds the low byte.
    tempData = (double)(((data_high & 0x007F) << 8) + data_low);
    tempData = (tempData * tempFactor)-0.01;
    
    float celcius = tempData - 273.15;
    float fahrenheit = (celcius*1.8) + 32;
    Serial.print("IR thermometer celcius: ");
    Serial.println(celcius);
    Serial.println();
    byte * tempBytes=(byte*)&celcius;
    outputFrame(idTemperatureIR,4,tempBytes); 
  }

  // read the value from the camera:

  if(enableCam){
    // TODO read camera data
    byte * camData;
    int camLength;
    //outputFrame(idCamera,camLength,camData); // uncomment when magdata ready

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

// ########## FRAME OUTPUT ##############

void outputFrame(int sensorID, int length, byte * data){
  /*
  Serial.print("Sensor: ");
  Serial.println(sensorID);
  Serial.print("Length: ");
  Serial.println(length);
  */
  // write frame to host (raspberry pi, pc or else)
  // frame bytes format with L in bytes.
  //   0   1   2-N
  // [Sid][L][Data]
  byte frame[length+2];
  // Write first byte (sensor ID) and second byte (length)
  frame[0] = (byte)(sensorID);
  frame[1] = (byte)length;
  // write remaining bytes (data bytes)
  memcpy(frame+2,data,length);
  
  // real communication
  //Serial.write(frame,length+2);
  
  // debug communication
  Serial.println("Frame: ");
  for(int i=0;i<length+2;i++){
    if(frame[i]<0x10)Serial.print("0");
    Serial.print(frame[i] & 0xFF,HEX);
  }
  Serial.println();
  Serial.println("------");
}


int ReadMagX(void)
{
  int xl, xh;  //define the MSB and LSB

  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x01);              // x MSB reg
  Wire.endTransmission();       // stop transmitting

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while(Wire.available())    // slave may write less than requested
  { 
    xh = Wire.read(); // read the byte
  }

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x02);              // x LSB reg
  Wire.endTransmission();       // stop transmitting

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while(Wire.available())    // slave may write less than requested
  { 
    xl = Wire.read(); // read the byte
  }

  int xout = (xl|(xh << 8)); //concatenate the MSB and LSB
  return xout;
}

int ReadMagY(void)
{
  int yl, yh;  //define the MSB and LSB

  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x03);              // y MSB reg
  Wire.endTransmission();       // stop transmitting

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while(Wire.available())    // slave may write less than requested
  { 
    yh = Wire.read(); // read the byte
  }

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x04);              // y LSB reg
  Wire.endTransmission();       // stop transmitting

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while(Wire.available())    // slave may write less than requested
  { 
    yl = Wire.read(); // read the byte
  }

  int yout = (yl|(yh << 8)); //concatenate the MSB and LSB
  return yout;
}

int ReadMagZ(void)
{
  int zl, zh;  //define the MSB and LSB

  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x05);              // z MSB reg
  Wire.endTransmission();       // stop transmitting

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while(Wire.available())    // slave may write less than requested
  { 
    zh = Wire.read(); // read the byte
  }

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x06);              // z LSB reg
  Wire.endTransmission();       // stop transmitting

  delayMicroseconds(2); //needs at least 1.3us free time between start and stop

  Wire.requestFrom(MAG_ADDR, 1); // request 1 byte
  while(Wire.available())    // slave may write less than requested
  { 
    zl = Wire.read(); // read the byte
  }

  int zout = (zl|(zh << 8)); //concatenate the MSB and LSB
  return zout;
}


