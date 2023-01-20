#define GPIO_PATH "/sys/class/gpio/gpio26/value"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>



void readFromFileToScreen(char *fileName)
{

    struct timespec remaining, request = { 1, 100 };
    while(1){
        FILE *pFile = fopen(fileName, "r");
        if (pFile == NULL) {
            printf("ERROR: Unable to open file (%s) for read\n", fileName);
            exit(-1);
        }
// Read string (line)
        const int MAX_LENGTH = 1024;
        char buff[MAX_LENGTH];
        fgets(buff, MAX_LENGTH, pFile);
// Close
        fclose(pFile);
        if (buff[0] == '0'){
            printf("Read: '%s'\n", buff);
        }

        nanosleep(&request, &remaining);
    }
}
int main() {
    readFromFileToScreen(GPIO_PATH);
}