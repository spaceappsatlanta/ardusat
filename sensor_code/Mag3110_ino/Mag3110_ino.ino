/*
  MAG3110 Breakout Example Code modified
  
  
  license: beerware, if you use this code and happen to meet me, you
           can by me a beer

  The code reads the raw 16-bit x, y, and z values and prints them 
  out. This sketch does not use the INT1 pin, nor does it poll for
  new data.

WIRING :
VCC 3.3V
SDA A4
SCL A5
GND...


*/

#include <Wire.h>

#define MAG_ADDR  0x0E //7-bit address for the MAG3110, doesn't change
char val; // variable to receive data from the serial port
void setup()
{
  Wire.begin();        // join i2c bus (address optional for master)
  Serial.begin(9600);  // start serial for output
  MagConfig();            // turn the MAG3110 on
}

void loop()
{
  get_values();
  delay(5);
  
  
  if( Serial.available() )       // if data is available to read
  {
    val = Serial.read();         // read it and store it in 'val'
	Serial.print(val);       
  }
  
}

void MagConfig(void)
{
  Serial.println("Magnetometer Setup...");
  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x11);              // cntrl register2
  Wire.write(0x80);              // write 0x80, enable auto resets
  Wire.endTransmission();       // stop transmitting
  
  delay(15);
  
  Wire.beginTransmission(MAG_ADDR); // transmit to device 0x0E
  Wire.write(0x10);              // cntrl register1
  Wire.write(1);                 // write 0x01, active mode
  Wire.endTransmission();       // stop transmitting
}


int get_values (void){
  
   float pi = 3.141597;
   int x = ReadMagX();
   int y = ReadMagY();
   int z = ReadMagZ();
   float heading = 0;
   
   
  if(y > 0){ 
    heading = 270 - atan2(x,y)*180 / pi; 
  }else if( y < 0 ) {
    heading = 90 - atan2(x,y)*180 / pi;
  } 
  if (y == 0 && x < 0) {heading = 180.0; }
  if (y == 0 && x > 0) {heading = 0.0; }
  
  print_values(x,y,z,heading);
}

void print_values(int x,int y,int z, float heading)
{
  Serial.print(" x=");
  Serial.print(x); 
  Serial.print(", ");  
  Serial.print(" y=");    
  Serial.print(y);
  Serial.print(", ");       
  Serial.print(" z=");    
  Serial.print(z); 
       
  Serial.print(" heading = ");    
  Serial.println(heading);  
    
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

