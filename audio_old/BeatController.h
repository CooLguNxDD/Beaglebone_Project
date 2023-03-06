// Playback sounds in real time, allowing multiple simultaneous wave files
// to be mixed together and played without jitter.
#ifndef BEAT_H
#define BEAT_H

#include <pthread.h>
#include "audioMixer_template.h"
#include "Utility.h"


#define DRUM_BD_HARD "beatbox-wav-file/100051__menegass__gui-drum-bd-hard.wav"
#define DRUM_CC "beatbox-wav-file/100053__menegass__gui-drum-cc.wav"
#define DRUM_SNARE_SOFT "beatbox-wav-file/100059__menegass__gui-drum-snare-soft.wav"


#define MIN_BPM 40
#define MAX_BPM 300

#define MAX_BEAT_SET 3
#define MAX_SOUND_SET 3
extern wavedata_t presetSound[MAX_SOUND_SET];

//thread
void startBeatController();
void clearBeatController();

//functions
void* startBeat();
bool playASound(int index);

//sound mode
void noDrum();
void standardRock();
void customSound();
void customSound2();

// set/get settings
bool setBPM(int bpm);
int getBPM();
int getBPMms(double beat);
void switchBeatSetting();
bool setBeatSetting(int setting);
int getBeatSetting();
#endif