#define LED0_TRIGGER_FILE_NAME "/sys/class/leds/beaglebone:green:usr0/trigger"
#define LED0_BRIGHTNESS_FILE_NAME "/sys/class/leds/beaglebone:green:usr0/brightness"
#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *LED0TriggerFile = fopen(LED0_TRIGGER_FILE_NAME, "w");
    if (LED0TriggerFile == NULL) {
        printf("ERROR OPENING %s \n", LED0_TRIGGER_FILE_NAME);
        exit(1);
    }
    FILE *LED0BrightnessFile = fopen(LED0_BRIGHTNESS_FILE_NAME, "w");
    if (LED0BrightnessFile == NULL) {
        printf("ERROR OPENING %s \n", LED0_BRIGHTNESS_FILE_NAME);
        exit(1);
    }

    int charWritten = fprintf(LED0TriggerFile, "none");
    if (charWritten <= 0) {
        printf("ERROR WRITING DATA");
        exit(1);
    }
    charWritten = fprintf(LED0BrightnessFile, "%d" , 1);

    fclose(LED0TriggerFile);
    fclose(LED0BrightnessFile);

}