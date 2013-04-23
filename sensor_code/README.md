# Arduino Source Code

The files located in the [sensor_code](https://github.com/spaceappsatlanta/ardusat/tree/master/sensor_code) sub-direcory can be loaded onto an [Ardiuno UNO](http://arduino.cc/en/Main/arduinoBoardUno) using the [Arduino IDE](http://arduino.cc/en/main/software). 

The file contained in the [PoorManSououz](https://github.com/spaceappsatlanta/ardusat/tree/master/sensor_code/PoorManSoyouz) sub-directory can be used to control the the following sensors concurrently:
- Digital Temperature Sensor (TMP 102)
- Inertial Unit: Gyroscope/ Accelerometer (ADXL345/ITG-3200)
- Digital Luminosity Light Sensor (TSL2561)
- Magnetometer (MAG3110)
- Infrared Thermometer (MLX90614)

In order for the sensors to work, the libraries contained in the [RequiredLibraries](https://github.com/spaceappsatlanta/ardusat/tree/master/sensor_code/RequiredLibraries) subdirecotry  must be added to the local Ardiuno Library folder. 

Additionally, sketches for individual sensors are also included in the [sensor_code](https://github.com/spaceappsatlanta/ardusat/tree/master/sensor_code) sub-direcory and labeled accordingly. 
