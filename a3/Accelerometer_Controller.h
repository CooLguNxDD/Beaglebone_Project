#ifndef ACC_CONTROLLER_H
#define ACC_CONTROLLER_H

#define I2CDRV_LINUX_BUS0 "/dev/i2c-1"
#define I2C_DEVICE_ADDRESS 0x1C

#include <pthread.h>
#include "Utility.h"


#define CTRL_REG1 0x2A
#define CTRL_REG1_CONFIG_0 0x00
#define CTRL_REG1_CONFIG_1 0x01

#define OUT_X_MSB "0x01"
#define OUT_X_MSB_CONFIG_0 "0x02"
#define OUT_X_MSB_CONFIG_1 "0x03"

#define OUT_Y_MSB "0x03"

#define OUT_Z_MSB "0x05"

//threads
void StartAccelerometerController();
void clearAccelerometerController();

//controllers
void* AccelerometerController();
void accelerationDetector(int i2cFileDesc, char* regAddr);
#endif