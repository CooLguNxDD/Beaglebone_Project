// App to test reading from files with different size buffers.
//
// Expected use:
//      readfile 10 /dev/myfile

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

int main(int argc, char** args)
{
    _Bool fail = false;
    if (argc != 3) {
        fail = true;
    }

    // Get the buffer size of interest:
    int buff_size = atoi(args[1]);
    if (buff_size <= 0) {
        printf("ERROR: Buffer size must be >= 1 byte.\n");
        printf("\n");
    }

    if (fail) {
        printf("Usage: \n");
        printf("    %s <Buffer Size> <File Name>\n", args[0]);
        printf("Example:\n");
        printf("    %s 256 /dev/testdriver\n", args[0]);
        return 0;
    }


    char *buff = malloc(sizeof(*buff) * (buff_size + 1));
    if (buff == NULL) {
        printf("Unable to allocate %d bytes for buffer.\n", buff_size);
        return 1;
    }

    int fd = open(args[2], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    int count = 0;
    while (1) {
        memset(buff, 0, buff_size + 1);
        int size_read = read(fd, buff, buff_size);

        if (size_read == -1) {
            perror("Error reading file");
            break;
        }

        printf(" --> Read #%-3d for %4d bytes:\n", count, size_read);
        // Check buffer integrity:
        if (buff[size_read] != 0) {
            printf("    ERROR: read() call filled more of buffer than it claimed!");
        }

        // Display buffer:
        printf("%s\n", buff);

        // Prep for next iteration
        count ++;

        if (size_read == 0) {
            break;
        }
    }

    close(fd);
    free(buff);
    return 0;
}