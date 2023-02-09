// Demo application to read analog input voltage 0 on the BeagleBone
// Assumes ADC cape already loaded by uBoot:


#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <string.h>

#include "Utility.h"
#include "HistoryArrayAlloc.h"
#include "periodTimer.h"
#include "LightSensorA2D.h"
#include "Global.h"

#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095
bool isSampling = false;
int lightSampleCount = 0;
int currentSample = 0;
int timer = 0;
int getVoltage1Reading()
{
// Open file
    FILE *f = fopen(A2D_FILE_VOLTAGE1, "r");
    if (!f) {
        printf("ERROR: Unable to open voltage input file. Cape loaded?\n");
        printf(" Check /boot/uEnv.txt for correct options.\n");
        exit(-1);
    }
// Get reading
    int a2dReading = 0;
    int itemsRead = fscanf(f, "%d", &a2dReading);
    if (itemsRead <= 0) {
        printf("ERROR: Unable to read values from voltage input file.\n");
        exit(-1);
    }
// Close file
    fclose(f);
    return a2dReading;
}

int SamplePerSecond = 0;

void* startReadA2D()
{
    while (isRunning) {
        if(isSampling){
            lightSampleCount += 1;
            int reading = getVoltage1Reading();
            currentSample = reading;
            Circular(reading);
            Period_markEvent(PERIOD_EVENT_SAMPLE_LIGHT);

            sleepForMs(1);
            SamplePerSecond ++;
            timer++;
            if(timer > 500){
                CountDown();
                timer = 0;
            }
        }
    }
    return 0;
}

void setStartSampling(){
    isSampling = true;
}

void setStopSampling(){
    isSampling = false;
}
int getNumOfSamples(){
    return lightSampleCount;
}
int getCurrentSample(){
    return currentSample;
}

