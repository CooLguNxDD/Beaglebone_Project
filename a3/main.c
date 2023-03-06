#include <stdio.h>
#include "Utility.h"
#include "audioMixer_template.h"
#include "BeatController.h"
#include "JoyStickController.h"
#include "Accelerometer_Controller.h"
#include "periodTimer.h"
#include "UDPSer.h"
#include <pthread.h>

Period_statistics_t pStatsAudio = {0,0,0,0};
Period_statistics_t pStatsAccel = {0,0,0,0};

void *printInfo(){
    while(isRunning){
        Period_getStatisticsAndClear(PERIOD_EVENT_AUDIO, &pStatsAudio);
        Period_getStatisticsAndClear(PERIOD_EVENT_ACCEL, &pStatsAccel);

        printf("M%d, %d bpm vol:%d, ", getBeatSetting(), getBPM(), AudioMixer_getVolume());
        printf("AUDIO[%0.3f, %0.3f] avg %0.3f/%d, ", pStatsAudio.minPeriodInMs, pStatsAudio.maxPeriodInMs, pStatsAudio.avgPeriodInMs, pStatsAudio.numSamples);
        printf("Accel[%0.3f, %0.3f] avg %0.3f/%d, ", pStatsAccel.minPeriodInMs, pStatsAccel.maxPeriodInMs, pStatsAccel.avgPeriodInMs, pStatsAccel.numSamples);

        printf("\n");
        sleepForMs(1000);
    }
    return NULL;
}

static pthread_t info;
void startLogging(){
    pthread_create(&info, NULL, &printInfo, NULL);
}

void endLogging(){
    pthread_join(info, NULL);
}

int main(void) {

    // Configure Output Device
//    snd_pcm_t *handle = Audio_openDevice();

    Period_init();


    startLogging();
    startBeatController();
    AudioMixer_init();
    startJoyStickController();
    StartAccelerometerController();
    startUDP();

    endLogging();
    stopUDP();
    clearBeatController();
    clearJoyStickController();
    AudioMixer_cleanup();
    clearAccelerometerController();




}
