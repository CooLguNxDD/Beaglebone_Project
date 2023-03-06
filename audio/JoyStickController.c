
#include "JoyStickController.h"
#include "Utility.h"
#include "BeatController.h"
#include "audioMixer_template.h"

//path to GPIO
char j_up[] = "/sys/class/gpio/gpio26/value";
char j_down[] = "/sys/class/gpio/gpio46/value";
char j_left[] = "/sys/class/gpio/gpio65/value";
char j_right[] = "/sys/class/gpio/gpio47/value";
char j_push[] = "/sys/class/gpio/gpio27/value";

static pthread_mutex_t lock;
static pthread_t up, down, left, right, push;


int clicked = 0;
char* clickedPosition;


void startJoyStickController(){


    runCommand("config-pin p8.14 gpio");
    runCommand("config-pin p8.15 gpio");
    runCommand("config-pin p8.16 gpio");
    runCommand("config-pin p8.18 gpio");
    runCommand("config-pin p8.17 gpio");

    runCommand("echo in > /sys/class/gpio/gpio26/direction");
    runCommand("echo in > /sys/class/gpio/gpio46/direction");
    runCommand("echo in > /sys/class/gpio/gpio65/direction");
    runCommand("echo in > /sys/class/gpio/gpio47/direction");
    runCommand("echo in > /sys/class/gpio/gpio27/direction");


    pthread_mutex_init(&lock, NULL);


    pthread_create(&up, NULL, DetectUserInputFromJoyStick, j_up);
    pthread_create(&down, NULL, DetectUserInputFromJoyStick, j_down);
    pthread_create(&left, NULL, DetectUserInputFromJoyStick, j_left);
    pthread_create(&right, NULL, DetectUserInputFromJoyStick, j_right);
    pthread_create(&push, NULL, DetectUserInputFromJoyStick, j_push);

}

void clearJoyStickController(){
    pthread_join(up, NULL);
    pthread_join(down, NULL);
    pthread_join(left, NULL);
    pthread_join(right, NULL);
    pthread_join(push, NULL);

}

void directionChecker(char * file){
    // up
    pthread_mutex_lock(&lock);
    if (file == j_up){
        AudioMixer_setVolume(AudioMixer_getVolume()+5);
    }
    // down
    else if (file == j_down){
        AudioMixer_setVolume(AudioMixer_getVolume()-5);
    }
    //left
    else if(file == j_left){
        setBPM(getBPM()-5);
    }
    //right
    else if(file == j_right){
        setBPM(getBPM()+5);
    }
    //push
    else if(file == j_push){
        switchBeatSetting();
    }
    pthread_mutex_unlock(&lock);

}

void* joyStickDelay(){
    sleepForMs(1000);
    clicked = 2;
    return NULL;
}
// detect input from GPIO pin of the joystick
void* DetectUserInputFromJoyStick(void *fileName)
{
    char *file = fileName;
    pthread_t delay;
    bool threadActive = false;
    while(isRunning){
        // Read string (line)
        FILE *pFile = fopen(file, "r");
        if (pFile == NULL) {
            printf("ERROR: Unable to open file (%s) for read\n", file);
            exit(-1);
        }

        const int MAX_LENGTH = 1024;
        char buff[MAX_LENGTH];
        fgets(buff, MAX_LENGTH, pFile);
        // Close file
        fclose(pFile);

        // if detected an input
        if (buff[0] == '0' && clicked == 0){
            directionChecker(file);

            pthread_mutex_lock(&lock);
            clicked = 1;
            clickedPosition = file;
            if(threadActive == false){
                pthread_create(&delay, NULL, joyStickDelay, NULL);
            }
            threadActive = true;
            pthread_mutex_unlock(&lock);
        }
        //long press delay thread
        else if(buff[0] == '1' && clickedPosition == fileName){

            pthread_mutex_lock(&lock);
            clicked = 0;
            if(threadActive == true){
                pthread_cancel(delay);
                pthread_join(delay, NULL);
            }
            threadActive = false;
            pthread_mutex_unlock(&lock);
        }
        //long press delay active (clicked = 2)
        else if(buff[0] == '0' && clicked == 2 && clickedPosition == fileName && fileName != j_push){
            directionChecker(file);
        }
        // a sleep to prevent intensive I/O
        sleepForMs(100);
    }
    return NULL;
}