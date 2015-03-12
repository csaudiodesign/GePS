# Arduino IDE 1.0.x

To add 12MHz support to SoftwareSerial.cpp on MacOS X, download the patch and
apply it with the following command:   

    https://raw.githubusercontent.com/csaudiodesign/GePS/minibee-hardware/arduino/SoftwareSerial-12MHz.patch
    patch /Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/avr/libraries/SoftwareSerial/SoftwareSerial.cpp < SoftwareSerial-12MHz.patch

# Arduino IDE 1.6.1

Download and install Arduino IDE 1.6.1

Install i2cdevlib:   

    git clone https://github.com/jrowberg/i2cdevlib.git
    cp -r i2cdevlib/Arduino/I2Cdev i2cdevlib/Arduino/MPU9150 ~/Documents/Arduino/libraries/

**minibee / sensestage package (our patched version):**   
patch the file MPU9150_9Axis_MotionApps41.h, it contains functions with wrong
names. The patch also increases the FIFO rate in order to reduce the polling
rate.    

    git clone https://github.com/csaudiodesign/ssdn_minibee.git
    patch ~/Documents/Arduino/libraries/MPU9150/MPU9150_9Axis_MotionApps41.h < ssdn_minibee/arduino/MPU9150_9Axis_MotionApps41.h.patch
    mkdir -p ~/Documents/Arduino/hardware
    cp -r ssdn_minibee/hardware-1.6/minibee ~/Documents/Arduino/hardware/

copy libraries as suggested by sense stage.   

    mkdir -p ~/Documents/Arduino/libraries
    cp -r ssdn_minibee/libraries/* ~/Documents/Arduino/libraries/

Since 1.6.x, the inclusion of Wire.h seems broken. This fixes it:   

    cp -r /Applications/Arduino.app/Contents/Resources/Java/hardware/arduino/avr/libraries/Wire ~/Documents/Arduino/libraries/

Open the sketch main_tx_sensordata.ino in the Arduino IDE. As of now, it's
possible to compile it, but upload still fails on the tested machine. WIP.

