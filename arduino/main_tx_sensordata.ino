#include "I2Cdev.h"
#include "MPU6050.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

///// INITIALIZE //////
// MPU
MPU6050 accelgyro;

// the 6 values of the MPU 
int16_t ax, ay, az;
int16_t gx, gy, gz;


///// BLUETOOTH START //////
// Include the software serial port library
#include <SoftwareSerial.h>

// DIO used to communicate with the Bluetooth module's TXD pin
#define BT_SERIAL_TX_DIO 10 

// DIO used to communicate with the Bluetooth module's RXD pin
#define BT_SERIAL_RX_DIO 11

// Initialise the software serial port
SoftwareSerial BluetoothSerial(BT_SERIAL_TX_DIO, BT_SERIAL_RX_DIO);

////// BLUETOOTH END ///////

void setup()
{
	// join I2C bus (I2Cdev library doesn't do this automatically)
	#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
	Wire.begin();
	#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
	Fastwire::setup(400, true);
	#endif
	
	// begin Software Serial Port
	BluetoothSerial.begin(38400);
	Serial.begin(38400);
	
	// initialize device
	accelgyro.initialize();
	
	accelgyro.setFullScaleGyroRange(32768);

	// accelgyro.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
}

// bit startpoint for parsing the serial data
int8_t start = 255;

void loop()
{
	// read raw accel/gyro measurements from device
	accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
	
	// values between -32768 and 32768 now go from 0 to 65536
	ax += 32768;
	ay += 32768;
	az += 32768;

	gx *= 0.99;		
	gy *= 0.99;		
	gz *= 0.99;		
 			 	
	gx += 32768;
	gy += 32768;
	gz += 32768;
	
	if (ax != 65535 && ay != 65535 && az != 65535)
	// 65535 is reserved for startpoint
	{
		BluetoothSerial.write((uint8_t) (start)   ); BluetoothSerial.write((uint8_t) (start)    );
		BluetoothSerial.write((uint8_t) (ax >> 8) ); BluetoothSerial.write((uint8_t) (ax & 255) );
		BluetoothSerial.write((uint8_t) (ay >> 8) ); BluetoothSerial.write((uint8_t) (ay & 255) );
		BluetoothSerial.write((uint8_t) (az >> 8) ); BluetoothSerial.write((uint8_t) (az & 255) );
		BluetoothSerial.write((uint8_t) (gx >> 8) ); BluetoothSerial.write((uint8_t) (gx & 255) );
		BluetoothSerial.write((uint8_t) (gy >> 8) ); BluetoothSerial.write((uint8_t) (gy & 255) );
		BluetoothSerial.write((uint8_t) (gz >> 8) ); BluetoothSerial.write((uint8_t) (gz & 255) );
	}
	
	if (ax != 65535 && ay != 65535 && az != 65535)
	// 65535 is reserved for startpoint
	//&& gx != 65535 && gy != 65535 && gz != 65535)
	{
		Serial.write((uint8_t)(start)); Serial.write((uint8_t)(start));
		Serial.write((uint8_t)(ax >> 8)); Serial.write((uint8_t)(ax & 255));
		Serial.write((uint8_t)(ay >> 8)); Serial.write((uint8_t)(ay & 255));
		Serial.write((uint8_t)(az >> 8)); Serial.write((uint8_t)(az & 255));
		Serial.write((uint8_t)(gx >> 8)); Serial.write((uint8_t)(gx & 255));
		Serial.write((uint8_t)(gy >> 8)); Serial.write((uint8_t)(gy & 255));
		Serial.write((uint8_t)(gz >> 8)); Serial.write((uint8_t)(gz & 255));
	}
}
