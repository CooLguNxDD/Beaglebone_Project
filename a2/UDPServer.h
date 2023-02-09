#ifndef UDP_H
#define UDP_H

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			// for strncmp()
#include <unistd.h>			// for close()

#define MSG_MAX_LEN 1024
#define PORT        12345

void* StartUDP();

#endif