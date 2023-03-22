#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "Utility.h"
#include "HistoryArrayAlloc.h"
#include "LightSensorA2D.h"


#define A2D_VOLTAGE_REF_V 1.8
#define A2D_MAX_READING 4095

int *historyArray;
int currentSize = 0;
int historySize = 0;
int newAverage;

int numOfDips = 0;

int head = 0;
int tail = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int numOfElements = 0;
bool isIdle;
void createArray(int size)
{
    newAverage = 0;
    int *arr = (int*)malloc(size * sizeof(int));

        for(int i= 0; i < size; i++)
        {
            arr[i] = 0;
        }
        currentSize = size;
        historyArray = arr;
        free(arr);
}

void ArrayReallocation(int targetSize){
	pthread_mutex_lock(&mutex);
	int *newArr = (int*)malloc(targetSize * sizeof(int));

	if(currentSize > targetSize){
		for(int i=0;i< targetSize;i++){
			newArr[i] = historyArray[i];
		}
	}
	else if(currentSize < targetSize){
		for(int i=0; i < currentSize; i++){
			newArr[i] = historyArray[i];
		}
	}
	historyArray = newArr;
	currentSize = targetSize;
    free(newArr);
	pthread_mutex_unlock(&mutex);
}

int dequeue(void) {
   int x = historyArray[head];
   head = (head + 1) % currentSize;
   return x;
}


void enqueue(int x) {
   historyArray[tail] = x;
   tail = (tail + 1) % currentSize;

   ComputeAverage(x);
   CheckDips(x);
}

void ComputeAverage(int new_number){
    newAverage = (int)((1 - SMOOTHING_ALPHA) * newAverage + SMOOTHING_ALPHA * new_number);
}

double ComputeVoltage(int reading){
	return ((double)reading / A2D_MAX_READING) * A2D_VOLTAGE_REF_V;
}

void Circular(int number){
    pthread_mutex_lock(&mutex);
    if(historySize < currentSize){
        historySize++;
    }
    else {
        historySize = currentSize;
    }
    enqueue(number);
    dequeue();
    pthread_mutex_unlock(&mutex);
}

void displayArray(){

	// printf("current Size is: %d \n", currentSize);

	for(int i = 0; i < currentSize;i+=1){
		printf("%d ", historyArray[i]);
		if(i%10 == 9){
			printf("\n");
		}
	}
}
int GetSampleAverage(){return newAverage;}
int* GetHistoryArray(){return historyArray;}
int GetHistorySize(){return historySize;}
int GetCurrentSampleSize(){return currentSize;}

int GetNumOfDips(){return numOfDips;}
void SetNumOfDips(int value){numOfDips = value;}

int dipsSample = 0;
bool trigger = true;
double trigger_threshold = THRESHOLD_VOLTAGE;

void CheckDips(int newSample){
    double voltageSample = ComputeVoltage(newAverage - newSample);
    //print(")
    if(voltageSample >= trigger_threshold){
        if(trigger){
//            pthread_mutex_lock(&mutex);
            numOfDips += 1;
//            pthread_mutex_unlock(&mutex);
            dipsSample = newAverage;
            isIdle = false;
            trigger = false;
            trigger_threshold = THRESHOLD_VOLTAGE + HYSTERESIS_VOLTAGE;
            timer = 0;
            //printf("dips: %d\n", GetNumOfDips());
        }
    }else if(voltageSample < trigger_threshold && fabs(ComputeVoltage(dipsSample - newSample)) < HYSTERESIS_VOLTAGE) {
        trigger = true;
        trigger_threshold = THRESHOLD_VOLTAGE;
    }*
    if(numOfDips >= 10){
        isIdle = true;
    }

}

void CountDown(){
    if(isIdle && numOfDips > 0 && trigger){
        pthread_mutex_lock(&mutex);
        numOfDips-=1;
        pthread_mutex_unlock(&mutex);
    }
}