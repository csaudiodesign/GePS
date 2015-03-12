// LIBRARIES (must be part of arduino's libraries)

#include <Wire.h>
#include <I2Cdev.h>                        // for i2c connection
#include <MPU9150_9Axis_MotionApps41.h>    // for MPU9150 sensor
#include <helper_3dmath.h>

// MPU SENSOR

// declare mpu sensor
MPU9150 mpu;
// control/status variables
uint16_t packetSize;    // expected DMP packet size
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer, default packet size: 48
// orientation/motion vars (necessary for )
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll array

// TRANSMISSION VARIABLES

// the 6 values we are interested in
int16_t yaw, pitch, roll;
int16_t gx, gy, gz;
// transmission helpers
uint8_t msbyaw, lsbyaw, msbpitch, lsbpitch, msbroll, lsbroll;
uint8_t msbgx, lsbgx, msbgy, lsbgy, msbgz, lsbgz;
// bit startpoint flag for parsing the serial data
int8_t start = 255;



////////////////////////////////////////////////////////
//////////////////////// SETUP /////////////////////////
////////////////////////////////////////////////////////

void setup() {
    // join I2C bus
    Wire.begin();
    // prevent fifo overflows by increasing i2c data rate
    TWBR = 12; // set 400kHz mode @ 16MHz CPU or 200kHz mode @ 8MHz CPU

    // initialize serial communication
    Serial.begin(38400);

    // initialize device
    mpu.initialize();
    mpu.dmpInitialize();
    mpu.setDMPEnabled(true);
    
    // set dmp sample rate to 8kHz
    mpu.setRate(0);
    
    // size of the data packet from the dmp. 48 bytes containing everything
    packetSize = mpu.dmpGetFIFOPacketSize();
    
    // lowest possible sensitivity to prevent clipping
    mpu.setFullScaleGyroRange(MPU9150_GYRO_FS_2000); // default: MPU6050_GYRO_FS_250
    mpu.setFullScaleAccelRange(MPU9150_ACCEL_FS_8); // default: MPU6050_ACCEL_FS_2
}



//////////////////////////////////////////////////////// 
//////////////////////// LOOP //////////////////////////
//////////////////////////////////////////////////////// 

void loop() {
  
    //////// COLLECT DATA ////////
    
    // how many bytes are in fifo?
    fifoCount = mpu.getFIFOCount();
    
    // wait until one packet is complete. THIS is the bottleneck
    while (fifoCount < packetSize) fifoCount = mpu.getFIFOCount();
    
    // read packet from FIFO
    mpu.getFIFOBytes(fifoBuffer, packetSize);
    
    // RESET MESSAGE MAY BE USEFUL
    // check for overflow (this should never happen unless our code is too inefficient)
    //if ((mpuIntStatus & 0x10) || fifoCount == 1024) {
        // reset so we can continue cleanly
    //mpu.resetFIFO();
    
    

    // compute YawPitchRoll
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    
    // read Raw Gyro Values
    mpu.getRotation(&gx, &gy, &gz);
    
    //////// TREAT FOR TRANSMISSION ////////
    
    // normalize & float to int (-π to π  =>  -32768 to 32768)
    yaw = (int)((ypr[0]/M_PI)*32768);
    pitch = (int)((ypr[1]/(M_PI*0.5))*32768);
    roll = (int)((ypr[2]/(M_PI*0.5))*32768);
    
    // signed int (-32768 and 32768) to unsigned int (0 to 65536)
    yaw += 32768;
    pitch += 32768;
    roll += 32768;

    gx += 32768;
    gy += 32768;
    gz += 32768;
    
    // cut off highest values (reserved for the "start" bytes)
    if (yaw == 65535) yaw = 65534;
    if (pitch == 65535) pitch = 65534;
    if (roll == 65535) roll = 65534;

    if (gx == 65535) gx = 65534;
    if (gy == 65535) gy = 65534;
    if (gz == 65535) gz = 65534;
    
    // send int16_t as 2 int8_ts for transmission and inhibit consecutive 255 bytes
    msbyaw = (uint8_t)(yaw >> 8); lsbyaw = (uint8_t)(yaw & 255);
    msbpitch = (uint8_t)(pitch >> 8); lsbpitch = (uint8_t)(pitch & 255);
    if(lsbyaw == 255 && msbpitch == 255) {
      lsbyaw = 254;
    }
    msbroll = (uint8_t)(roll >> 8); lsbroll = (uint8_t)(roll & 255);
    if(lsbpitch == 255 && msbroll == 255) {
      lsbpitch = 254;
    }
    msbgx = (uint8_t)(gx >> 8); lsbgx = (uint8_t)(gx & 255);
    if(lsbroll == 255 && msbgx == 255) {
      lsbroll = 254;
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
    Serial.write(msbyaw); Serial.write(lsbyaw);
    Serial.write(msbpitch); Serial.write(lsbpitch);
    Serial.write(msbroll); Serial.write(lsbroll);
    Serial.write(msbgx); Serial.write(lsbgx);
    Serial.write(msbgy); Serial.write(lsbgy);
    Serial.write(msbgz); Serial.write(lsbgz);
}
