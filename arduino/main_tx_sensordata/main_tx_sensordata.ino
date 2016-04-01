// LIBRARIES (must be part of arduino's libraries)

#include <Wire.h>
#include <I2Cdev.h>                        // for i2c connection
#include <MPU9150.h>                       // for MPU9150 / MPU6050 sensor
#include <helper_3dmath.h>

// MPU SENSOR

// declare mpu sensor (also works with MPU6050 Hardware)
MPU9150 mpu;

// TRANSMISSION VARIABLES

// the 6 values we are interested in
int16_t ax, ay, az;
int16_t gx, gy, gz;
// transmission helpers
uint8_t msbax, lsbax, msbay, lsbay, msbaz, lsbaz;
uint8_t msbgx, lsbgx, msbgy, lsbgy, msbgz, lsbgz;
// bit startpoint flag for parsing the serial data
int8_t start = 255;



////////////////////////////////////////////////////////
//////////////////////// SETUP /////////////////////////
////////////////////////////////////////////////////////

void setup() {
    // join I2C bus
    Wire.begin();

    // initialize serial communication
    Serial.begin(38400);

    // initialize device
    mpu.initialize();
    
    // lowest possible sensitivity to prevent clipping
    mpu.setFullScaleGyroRange(MPU9150_GYRO_FS_2000); // default: MPU6050_GYRO_FS_250
    mpu.setFullScaleAccelRange(MPU9150_ACCEL_FS_8); // default: MPU6050_ACCEL_FS_2
}



////////////////////////////////////////////////////////
//////////////////////// LOOP //////////////////////////
//////////////////////////////////////////////////////// 

void loop() {
  
    //////// COLLECT DATA ////////
    
    // read Sensor Values
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    //////// TREAT FOR TRANSMISSION ////////
    
    // signed int (-32768 and 32768) to unsigned int (0 to 65535)
    ax += 32768;
    ay += 32768;
    az += 32768;

    gx += 32768;
    gy += 32768;
    gz += 32768;
    
    // cut off highest values (reserved for the "start" bytes)
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
    
    // send the values
    Serial.write((uint8_t)(start)); Serial.write((uint8_t)(start));
    Serial.write(msbax); Serial.write(lsbax);
    Serial.write(msbay); Serial.write(lsbay);
    Serial.write(msbaz); Serial.write(lsbaz);
    Serial.write(msbgx); Serial.write(lsbgx);
    Serial.write(msbgy); Serial.write(lsbgy);
    Serial.write(msbgz); Serial.write(lsbgz);

    
    delay(2); // we do not need a polling rate faster than 5 ms. give the guy a break.
}
