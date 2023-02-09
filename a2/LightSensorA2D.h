#ifndef LIGHT_SENSOR_A2D_H
#define LIGHT_SENSOR_A2D_H

#define A2D_FILE_VOLTAGE1 "/sys/bus/iio/devices/iio:device0/in_voltage1_raw"


#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include <time.h>
#include <string.h>

void setStartSampling();
void setStopSampling();
extern int SamplePerSecond;
int getVoltage1Reading();
extern int timer;

void* startReadA2D();

int getNumOfSamples();
int getCurrentSample();
#endif