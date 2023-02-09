#ifndef POT_H
#define POT_H

#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "HistoryArrayAlloc.h"
#include "Utility.h"


#define A2D_FILE_VOLTAGE0 "/sys/bus/iio/devices/iio:device0/in_voltage0_raw"
#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

int getVoltage0Reading();
void* getPotVoltageReading();

#endif


