
#include "PotDrivers.h"
#include "Global.h"


int getVoltage0Reading()
{
// Open file
    FILE *f = fopen(A2D_FILE_VOLTAGE0, "r");
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

#include <time.h>
void* getPotVoltageReading(){
    while (isRunning) {
        int reading = getVoltage0Reading();
        if(reading == 0){
            ArrayReallocation(1);
        }
        else{
            ArrayReallocation(reading);
        }

        sleepForMs(1000);
    }
    return NULL;
}

