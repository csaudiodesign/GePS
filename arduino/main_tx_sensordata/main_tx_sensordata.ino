#include "I2Cdev.h"
#include "MPU9150.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

///// INITIALIZE //////
// MPU
MPU9150 mpu;

// the 6 values of the MPU
int16_t ax, ay, az;
int16_t gx, gy, gz;

// bit startpoint for parsing the serial data
uint8_t start = 255;

// MSB/LSB variables for encapsulation
uint8_t msbax, lsbax, msbay, lsbay, msbaz, lsbaz;
uint8_t msbgx, lsbgx, msbgy, lsbgy, msbgz, lsbgz;

///// XBEE SERIAL CONNECTION //////
// Include the software serial port library
#include <SoftwareSerial.h>

// DIO used to communicate with the Bluetooth module's TXD pin
#define BT_SERIAL_TX_DIO 10 

// DIO used to communicate with the Bluetooth module's RXD pin
#define BT_SERIAL_RX_DIO 11

// Initialise the software serial port
SoftwareSerial XBeeSerial(BT_SERIAL_TX_DIO, BT_SERIAL_RX_DIO);

void setup()
{
	// join I2C bus (I2Cdev library doesn't do this automatically)
	#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
	Wire.begin();
	#elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
	Fastwire::setup(400, true);
	#endif

	// begin Software Serial Port
	XBeeSerial.begin(38400);

	// initialize device
	mpu.initialize();

	mpu.setFullScaleGyroRange(MPU9150_GYRO_FS_2000); // default: MPU6050_GYRO_FS_250
	mpu.setFullScaleAccelRange(MPU9150_ACCEL_FS_8);
}

void loop()
{
	// read raw accel/gyro measurements from device
	mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
	
	// convert sensor values from int to uint (0...65535)
	ax += 32768;
	ay += 32768;
	az += 32768;
 			 	
	gx += 32768;
	gy += 32768;
	gz += 32768;

	// cut off highest values (2^16-1, reserved for the "start" bytes)
	if (ax == 65535) ax = 65534;
	if (ay == 65535) ay = 65534;
	if (az == 65535) az = 65534;

	if (gx == 65535) gx = 65534;
	if (gy == 65535) gy = 65534;
	if (gz == 65535) gz = 65534;

	// send int16_t as 2 int8_ts for transmission and inhibit consecutive 255 bytes
	msbax = (uint8_t)(ax >> 8); lsbax = (uint8_t)(ax & 255);
	msbay = (uint8_t)(ay >> 8); lsbay = (uint8_t)(ay & 255);
	if(lsbax == 255 && msbay == 255) {
		lsbax = 254;
	}
	msbaz = (uint8_t)(az >> 8); lsbaz = (uint8_t)(az & 255);
	if(lsbay == 255 && msbaz == 255) {
		lsbay = 254;
	}
	msbgx = (uint8_t)(gx >> 8); lsbgx = (uint8_t)(gx & 255);
	if(lsbaz == 255 && msbgx == 255) {
		lsbaz = 254;
	}
	msbgy = (uint8_t)(gy >> 8); lsbgy = (uint8_t)(gy & 255);
	if(lsbgx == 255 && msbgy == 255) {
		lsbgx = 254;
	}
	msbgz = (uint8_t)(gz >> 8); lsbgz = (uint8_t)(gz & 255);
	if(lsbgy == 255 && msbgz == 255) {
		lsbgy = 254;
	}
	if(lsbgz == 255) {
		lsbgz = 254;
	}

	// transmit values
	XBeeSerial.write(start); XBeeSerial.write(start);
	XBeeSerial.write(msbax); XBeeSerial.write(lsbax);
	XBeeSerial.write(msbay); XBeeSerial.write(lsbay);
	XBeeSerial.write(msbaz); XBeeSerial.write(lsbaz);
	XBeeSerial.write(msbgx); XBeeSerial.write(lsbgx);
	XBeeSerial.write(msbgy); XBeeSerial.write(lsbgy);
	XBeeSerial.write(msbgz); XBeeSerial.write(lsbgz);
}
