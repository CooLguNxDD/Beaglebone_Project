#ifndef JOYSTICK_H
#define JOYSTICK_H

#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



//function
void* DetectUserInputFromJoyStick(void *fileName);

//thread
void startJoyStickController();
void clearJoyStickController();

//function
void directionChecker(char * file);


#endif
