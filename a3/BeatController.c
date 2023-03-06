#include <stdio.h>
#include "BeatController.h"


int beatSetting;
static int bpm;

wavedata_t presetSound[MAX_BEAT_SET];

static pthread_t beating;
void startBeatController(){
    pthread_create(&beating, NULL, &startBeat, NULL);
}

void clearBeatController(){
    pthread_join(beating, NULL);
}
void* startBeat(){

    runCommand("config-pin p9_18 i2c");
    runCommand("config-pin p9_17 i2c");

    // Load wave file we want to play:
    wavedata_t drum_bd_hard;
    AudioMixer_readWaveFileIntoMemory(DRUM_BD_HARD, &drum_bd_hard);

    wavedata_t drum_cc;
    AudioMixer_readWaveFileIntoMemory(DRUM_CC, &drum_cc);

    wavedata_t drum_snare_soft;
    AudioMixer_readWaveFileIntoMemory(DRUM_SNARE_SOFT, &drum_snare_soft);


    presetSound[0] = drum_bd_hard;
    presetSound[1] = drum_cc;
    presetSound[2] = drum_snare_soft;

    beatSetting = 1;
    setBPM(120);

    while (isRunning) {
        if(beatSetting == 0){
            noDrum();
        }
        if(beatSetting == 1){
            standardRock();
            if(beatSetting != 1) continue;
            standardRock();
        }
        if(beatSetting == 2){
            customSound();
            if(beatSetting != 2) continue;
            customSound2();
        }
    }
    return NULL;
}

bool playASound(int index){
    if (index >= 0 && index < MAX_SOUND_SET){
        AudioMixer_queueSound(&presetSound[index]);
        return true;
    }
    return false;
}

void noDrum() {
//    AudioMixer_queueSound(&presetSound[0]);
    sleepForMs(getBPMms(1));
}
void standardRock(){
    AudioMixer_queueSound(&presetSound[1]);
    AudioMixer_queueSound(&presetSound[0]);
    sleepForMs(getBPMms(0.5));

    AudioMixer_queueSound(&presetSound[1]);
    sleepForMs(getBPMms(0.5));

    AudioMixer_queueSound(&presetSound[1]);
    AudioMixer_queueSound(&presetSound[2]);
    sleepForMs(getBPMms(0.5));

    AudioMixer_queueSound(&presetSound[1]);
    sleepForMs(getBPMms(0.5));

}
void customSound(){
    AudioMixer_queueSound(&presetSound[0]);
    AudioMixer_queueSound(&presetSound[1]);
    sleepForMs(getBPMms(1));
    AudioMixer_queueSound(&presetSound[0]);
    AudioMixer_queueSound(&presetSound[1]);
    sleepForMs(getBPMms(1));
    AudioMixer_queueSound(&presetSound[0]);
    AudioMixer_queueSound(&presetSound[1]);
    sleepForMs(getBPMms(1));
}

void customSound2(){
    AudioMixer_queueSound(&presetSound[0]);
    AudioMixer_queueSound(&presetSound[1]);
    sleepForMs(getBPMms(0.5));
    AudioMixer_queueSound(&presetSound[0]);
    sleepForMs(getBPMms(0.5));
    AudioMixer_queueSound(&presetSound[2]);
    AudioMixer_queueSound(&presetSound[1]);
    sleepForMs(getBPMms(0.5));
    AudioMixer_queueSound(&presetSound[0]);
    sleepForMs(getBPMms(0.5));
    AudioMixer_queueSound(&presetSound[1]);
    AudioMixer_queueSound(&presetSound[0]);
    sleepForMs(getBPMms(0.5));
    AudioMixer_queueSound(&presetSound[2]);
    sleepForMs(getBPMms(0.5));
}

bool setBPM(int newBpm){
    if(MIN_BPM > newBpm || MAX_BPM < newBpm){
        //use for get value without prompt
        if(newBpm != -1000){
            printf("ERROR: BPM must be between %d and %d.\n", MIN_BPM, MAX_BPM);
        }
        return false;
    }
    bpm = newBpm;

    printf("current bpm: %d \n", bpm);
    return true;
}

int getBPM(){
    return bpm;
}

int getBPMms(double beat){
    double min = 60.0;
    return (int)(min / ((double)bpm) * 1000 * beat);
}
void switchBeatSetting(){
    beatSetting += 1;
    if(beatSetting >= MAX_BEAT_SET){
        beatSetting = 0;
    }
    printf("current beat: %d \n", beatSetting);
}
bool setBeatSetting(int setting){
    if (setting >=0 && setting < MAX_BEAT_SET){
        beatSetting = setting;
        return true;
    }
    return false;

}
int getBeatSetting(){
    return beatSetting;
}


