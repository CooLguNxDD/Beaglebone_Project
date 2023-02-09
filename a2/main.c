
#include <stdio.h>
#include "PotDrivers.h"
#include "HistoryArrayAlloc.h"
#include "LightSensorA2D.h"
#include "HistoryArrayAlloc.h"
#include <pthread.h>
#include "Global.h"
#include "periodTimer.h"
#include "I2C.h"
#include "UDPServer.h"


Period_statistics_t pStats = {0,0,0,0};

void *printInfo(){
    while(isRunning){
        int validSize = getNumOfSamples();
        if(validSize > GetCurrentSampleSize()){
            validSize = GetCurrentSampleSize();
        }

        Period_getStatisticsAndClear(PERIOD_EVENT_SAMPLE_LIGHT, &pStats);

        printf("Sample/s: #%d, PotValue: %d, history size: %d, avg: %.03fV, dips: %d ", SamplePerSecond, GetCurrentSampleSize(), GetHistorySize(),
               ComputeVoltage(GetSampleAverage()), GetNumOfDips());
        printf("Sampling[%0.3f, %0.3f] avg %0.3f/{%d}", pStats.minPeriodInMs, pStats.maxPeriodInMs, pStats.avgPeriodInMs, pStats.numSamples);

        printf("\n");

        int* array = GetHistoryArray();

        for(int i=validSize;i > 0;i-=200){
            printf("%0.3f, ", ComputeVoltage(array[i]));
        }
        printf("\n");
        SamplePerSecond = 0;
        sleepForMs(1000);
    }
    return NULL;
}

int main() {

    runCommand("sudo config-pin P9_18 i2c");
    runCommand("sudo config-pin P9_17 i2c");

    runCommand("sudo echo 61 > /sys/class/gpio/export");
    runCommand("sudo echo 61 > /sys/class/gpio/export");

    runCommand("sudo echo 44 > /sys/class/gpio/export");
    runCommand("sudo echo 44 > /sys/class/gpio/export");


    runCommand("sudo echo out > /sys/class/gpio/gpio61/direction");
    runCommand("sudo echo out > /sys/class/gpio/gpio44/direction");

    runCommand("sudo echo 1 > /sys/class/gpio/gpio61/value");
    runCommand("sudo echo 1 > /sys/class/gpio/gpio44/value");

    Period_init();

    createArray(HISTORY_SIZE);
    setStartSampling();
    pthread_t DataRecording, tPrintInfo, potInfo , runI2C ,tUDP, tLED;

    pthread_create(&tLED, NULL, &setLED, NULL);
    pthread_create(&DataRecording, NULL, &startReadA2D, NULL);
    pthread_create(&tPrintInfo, NULL, &printInfo, NULL);
    pthread_create(&potInfo, NULL, &getPotVoltageReading, NULL);
    pthread_create(&runI2C, NULL, &RunI2C, NULL);

    pthread_create(&tUDP, NULL, &StartUDP, NULL);

    pthread_join(tUDP, NULL);
    pthread_join(tLED,NULL);
    pthread_join(runI2C, NULL);
    pthread_join(potInfo, NULL);
    pthread_join(tPrintInfo, NULL);
    pthread_join(DataRecording, NULL);

    runCommand("sudo echo 1 > /sys/class/gpio/gpio61/value");
    runCommand("sudo echo 1 > /sys/class/gpio/gpio44/value");

}
