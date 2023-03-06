#ifndef UTILITY_H
#define UTILITY_H

#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdbool.h>


extern bool isRunning;

void sleepForMs(long long delayInMs);
void runCommand(char* command);
int initI2cBus(char* bus, int address);
void writeI2cReg(int i2cFileDesc, unsigned char regAddr,
                 unsigned char value);
#endif