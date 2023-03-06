/* udpComms.h
*
*  Module for the networking component of the audio visualizer program.
*/

#ifndef _UDP_H_
#define _UDP_H_

// modules required
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <netdb.h>
#include <unistd.h>			// for close()

#include <pthread.h>

// const
#define MSG_MAX_LEN 1024
#define PORT        12345


// Init UDP.
void startUDP(void);

// Cleans up UDP.
void stopUDP(void);

// Initializes the UDP server
void* StartUDPServer();
void* StartPostServer();

void getSystemUpTime(char * messageTx);
#endif