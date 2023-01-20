
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//path to GPIO
char j_up[] = "/sys/class/gpio/gpio26/value";
char j_down[] = "/sys/class/gpio/gpio46/value";
char j_left[] = "/sys/class/gpio/gpio65/value";
char j_right[] = "/sys/class/gpio/gpio47/value";

char led0_trigger[] = "/sys/class/leds/beaglebone:green:usr0/trigger";
char led1_trigger[] = "/sys/class/leds/beaglebone:green:usr1/trigger";
char led2_trigger[] = "/sys/class/leds/beaglebone:green:usr2/trigger";
char led3_trigger[] = "/sys/class/leds/beaglebone:green:usr3/trigger";
char led0_brightness[] = "/sys/class/leds/beaglebone:green:usr0/brightness";
char led1_brightness[] = "/sys/class/leds/beaglebone:green:usr1/brightness";
char led2_brightness[] = "/sys/class/leds/beaglebone:green:usr2/brightness";
char led3_brightness[] = "/sys/class/leds/beaglebone:green:usr3/brightness";

//global
int active = 0;
int clicked = 0;
int light_up = 0;
int random_direction = 0;

//record
long long best_time = 99999999;

//utilities

//get current time
static long long getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000
                             + nanoSeconds / 1000000;
    return milliSeconds;
}
//set delay
static void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}
// run a linux command
static void runCommand(char* command)
{
    // Execute the shell command (output into pipe)
    FILE *pipe = popen(command, "r");
    // Ignore output of the command; but consume it
    // so we don't get an error when closing the pipe.
    char buffer[1024];
    while (!feof(pipe) && !ferror(pipe)) {
        if (fgets(buffer, sizeof(buffer), pipe) == NULL)
            break;
        // printf("--> %s", buffer); // Uncomment for debugging
    }
    // Get the exit code from the pipe; non-zero is an error:
    int exitCode = WEXITSTATUS(pclose(pipe));
    if (exitCode != 0) {
        perror("Unable to execute command:");
        printf(" command: %s\n", command);
        printf(" exit code: %d\n", exitCode);
    }
}

// set the LEDs
// arg1: path to GPIO pin for LED
// arg2: option
void LEDLightSetting(char* file_name, char* light){

    FILE *LED = fopen(file_name, "w");
    if (LED == NULL) {
        printf("ERROR OPENING %s \n", file_name);
        exit(1);
    }

    fprintf(LED, "%s" , light);
    fclose(LED);
}
// reset global value for a new game
void ResetGlobalValue(){
    active = 0;
    clicked = 0;
    light_up = 0;
    random_direction = 0;
    LEDLightSetting(led0_brightness,"0");
    LEDLightSetting(led1_brightness,"1");
    LEDLightSetting(led2_brightness,"1");
    LEDLightSetting(led3_brightness,"0");
}

// check which direction user selected on base on the file path
// enable game logic
void directionChecker(char* file){
    // up
    if (file == j_up){
        active = 1;
    }
    // down
    if (file == j_down){
        active = 2;
    }
    //quit game
    if(file == j_left || file == j_right){
        printf("User selected to quit. \n");
        exit(1);
    }
    // clicked too soon!
    if(light_up == 0){
        printf("Too soon! \n");
    }
}

// detect input from GPIO pin of the joystick
void* DetectUserInputFromJoyStick(void *fileName)
{
    char *file =  fileName;
    while(1){
        FILE *pFile = fopen(file, "r");
        if (pFile == NULL) {
            printf("ERROR: Unable to open file (%s) for read\n", file);
            exit(-1);
        }
        // Read string (line)
        const int MAX_LENGTH = 1024;
        char buff[MAX_LENGTH];
        fgets(buff, MAX_LENGTH, pFile);
        // Close file

        fclose(pFile);
        // if detected an input
        if (buff[0] == '0'){
            directionChecker(file);
            clicked = 1;
        }
        // end loop if ONE input detected
        if(clicked == 1){
            return NULL;
        }

        // a sleep to prevent intensive I/O
        sleepForMs(100);
    }
}
void LEDFlash(float duration, float freq) {

    int time_interval = (int) ((duration / freq) * 1000);
    int i = 0;
    while (1) {
        i += time_interval * 2;
        if ((int)(duration * 1000) < i) {
            break;
        }

        LEDLightSetting(led0_brightness, "0");
        LEDLightSetting(led1_brightness, "0");
        LEDLightSetting(led2_brightness, "0");
        LEDLightSetting(led3_brightness, "0");

        sleepForMs(time_interval);

        LEDLightSetting(led0_brightness, "1");
        LEDLightSetting(led1_brightness, "1");
        LEDLightSetting(led2_brightness, "1");
        LEDLightSetting(led3_brightness, "1");

        sleepForMs(time_interval);
    }
    LEDLightSetting(led0_brightness, "0");
    LEDLightSetting(led1_brightness, "0");
    LEDLightSetting(led2_brightness, "0");
    LEDLightSetting(led3_brightness, "0");
}
// set record for user
void record(long long current_time){

    if((active == 1 && random_direction == 1) ||
            (active == 2 && random_direction == 2)){
        printf("Correct! \n");
        LEDFlash(0.5f,4);
        if (current_time < best_time){
            printf("New Best Time!!!!! \n");
            best_time = current_time;
        }
        printf("Your reaction time was %lld ms; best so far in game is %lld ms. \n", current_time, best_time);
    }
    else{
        printf("Wrong Direction! \n");
        LEDFlash(1,10);
    }

}
// exit program if user is inactive for a certain time
void* InactiveDetection(void* time){
    long long sleep_time = *(long long*)time;
    sleepForMs(sleep_time);
    if(clicked == 0){
        printf("User is Inactive, exiting...... \n");
        exit(1);
    }
    return NULL;
}


// game LED
void* GameLED( void * temp){
    int random_time = 500+ rand() % 3000;
    random_direction = 1+rand() % 2;



    sleepForMs(random_time);

    pthread_t inactive;
    long long new_time = 5000;
    pthread_create(&inactive, NULL, &InactiveDetection, &new_time);

    LEDLightSetting(led1_brightness,"0");
    LEDLightSetting(led2_brightness,"0");

    long long start_time = getTimeInMs();
    if(clicked == 0){
        if (random_direction == 1){
            printf("press UP now! \n");
            LEDLightSetting(led0_brightness,"1");
            light_up = 1;
        }
        else if(random_direction == 2){
            printf("press DOWN now! \n");
            LEDLightSetting(led3_brightness,"1");
            light_up = 1;
        }
    }
    else{
        return NULL;
    }

    while(light_up == 1){
        if (clicked == 1){
            light_up = 0;
            long long end_time = getTimeInMs();
            record(end_time-start_time);
        }
    }

    pthread_cancel(inactive);
    return NULL;
}

// all the threads
void startGameThread(){

    pthread_t LED, up, down, left, right;

    pthread_create(&LED, NULL, &GameLED, NULL);
    pthread_create(&up, NULL, &DetectUserInputFromJoyStick, j_up);
    pthread_create(&down, NULL, &DetectUserInputFromJoyStick, j_down);
    pthread_create(&left, NULL, &DetectUserInputFromJoyStick, j_left);
    pthread_create(&right, NULL, &DetectUserInputFromJoyStick, j_right);

    pthread_join(up, NULL);
    pthread_join(down, NULL);
    pthread_join(left, NULL);
    pthread_join(right, NULL);
    pthread_join(LED, NULL);

}

// start game
void start(){
    while (1){
        printf("get ready \n");
        ResetGlobalValue();
        startGameThread();
        sleepForMs(1000);
    }
}

//main loop for pre-setting
int main(){
    printf("Hello embedded world, from Andrew Liang! \n \n");
    printf("When the LEDs light up, press the joystick in that direction!\n"
"(Press left or right to exit) \n \n");

    runCommand("config-pin p8.14 gpio");
    runCommand("config-pin p8.15 gpio");
    runCommand("config-pin p8.16 gpio");
    runCommand("config-pin p8.18 gpio");

    LEDLightSetting(led0_trigger,"none");
    LEDLightSetting(led1_trigger,"none");
    LEDLightSetting(led2_trigger,"none");
    LEDLightSetting(led3_trigger,"none");

    start();
    return 0;
}

