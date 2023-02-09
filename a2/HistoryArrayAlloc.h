#ifndef ARRAY_ALLOC_H
#define ARRAY_ALLOC_H
#include <stdio.h>
#include <pthread.h>

#define SMOOTHING_ALPHA 0.01
#define HISTORY_SIZE 1000

#define THRESHOLD_VOLTAGE 0.1
#define HYSTERESIS_VOLTAGE 0.03

void createArray(int size);
void ArrayReallocation(int targetSize);
void Circular(int number);

void ComputeAverage(int new_number);

void displayArray();
int GetSampleAverage();
int* GetHistoryArray();
int GetCurrentSampleSize();
int GetHistorySize();

double ComputeVoltage();

int GetNumOfDips();
void SetNumOfDips(int value);
void CheckDips(int newSample);

void CountDown();
#endif
