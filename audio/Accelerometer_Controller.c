

#include <stdbool.h>
#include "Accelerometer_Controller.h"
#include "audioMixer_template.h"
#include "BeatController.h"
#include "periodTimer.h"
static pthread_t accelerometer;

void StartAccelerometerController(){
    pthread_create(&accelerometer, NULL, &AccelerometerController, NULL);
}

void clearAccelerometerController(){
    pthread_join(accelerometer, NULL);
}


void* AccelerometerController(){

    //active i2c
    int i2cFileDesc = initI2cBus(I2CDRV_LINUX_BUS0, I2C_DEVICE_ADDRESS);
    writeI2cReg(i2cFileDesc, CTRL_REG1, CTRL_REG1_CONFIG_0);
    writeI2cReg(i2cFileDesc, CTRL_REG1, CTRL_REG1_CONFIG_1);

    while(isRunning){
        Period_markEvent(PERIOD_EVENT_ACCEL);
        accelerationDetector(i2cFileDesc, OUT_X_MSB);
//        accelerationDetector(i2cFileDesc, OUT_Y_MSB);
//        accelerationDetector(i2cFileDesc, OUT_Z_MSB);
        sleepForMs(10);
    }

    close(i2cFileDesc);
    return 0;
}

//store previous
// played flag to prevent re-trigger


int previousCounterX = 0;
int previousCounterY = 0;
int previousCounterZ = 1000;

//threshold for short burst
int threshold = 600;

void accelerationDetector(int i2cFileDesc, char* regAddr){

    // read 7 bytes
    int size = 7;
    char data[7];
    memset(data, 0, sizeof(data));

    //start read!
    write(i2cFileDesc, regAddr, 2);
    read(i2cFileDesc, data, size);

    int X_value, Y_value, Z_value;

    //first and last 2 Bytes are useless
    for( int i=1; i<size-1; i+=2){
        //change 4 bytes to decimal
        int counter = (data[i] << 8) | data[i+1];

        // it's left-aligns with 12 bits, need to shift 4 bits
        counter >>= 4;

        // the value is negative 0x80 is -128, 0x7F is 127
        // change it to negative deciaml number when more than 0x7F
        if(data[i] > 0x7F) counter -= 0x1000;

        //left right
        if(i/2 == 0){
            if(abs(previousCounterX - counter) > threshold){
//                printf("counter: i = %d value: %d, previousX: %d ", i/2, counter, previousCounterX);
                X_value = abs(counter);
            }
            else{
                X_value = -1;
            }
            previousCounterX = counter;
        }
        //away towards
        else if(i/2 == 1){
            if(abs(previousCounterY - counter) > threshold){
//                printf("counter: i = %d value: %d, previousY: %d ", i/2, counter, previousCounterY);
                Y_value = abs(counter);
            }
            else{
                Y_value = -1;
            }
            previousCounterY = counter;
        }
        // up down
        else if(i/2 == 2){
            if(abs(previousCounterZ - counter) > threshold){
//                printf("counter: i = %d value: %d, previousZ: %d ", i/2, counter, previousCounterZ);
                Z_value = abs(counter - 1000);
            }
            else{
                Z_value = false;
            }
            previousCounterZ = counter;
        }
    }

    // only play a sound with the largest burst
    if(X_value > threshold && X_value > Y_value && X_value > Z_value){
        AudioMixer_queueSound(&presetSound[1]);
        sleepForMs(getBPMms(1));
    }
    else if(Y_value > threshold && Y_value > X_value && Y_value > Z_value){
        AudioMixer_queueSound(&presetSound[2]);
        sleepForMs(getBPMms(1));
    }
    else if(Z_value > threshold && Z_value > X_value && Z_value > Y_value){
        AudioMixer_queueSound(&presetSound[0]);
        sleepForMs(getBPMms(1));
    }
}