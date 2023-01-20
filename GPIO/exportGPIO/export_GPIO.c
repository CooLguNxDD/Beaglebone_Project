#define GPIO_PATH "/sys/class/gpio/export"

#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *ExportFile = fopen(GPIO_EXPORT_PATH, "w");
    if (ExportFile == NULL) {
        printf("ERROR OPENING %s \n", GPIO_EXPORT_PATH);
        exit(1);
    }

    fprintf(ExportFile, "%d", 30);

    fclose(ExportFile);
}