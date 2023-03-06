#include "UDPSer.h"
#include "../Final_project/cmpt433-final-project/src/Utility.h"

#include "../Final_project/cmpt433-final-project/src/audioMixer/BeatController.h"
#include "../Final_project/cmpt433-final-project/src/audioMixer/audioMixer_template.h"

// previous command
char previousCmd[MSG_MAX_LEN];


static pthread_t UDP;
//static pthread_t PostMessage;
// Initializes the UDP thread.
void startUDP(void) {

    pthread_create(&UDP, NULL, StartUDPServer, NULL);
//    pthread_create(&PostMessage, NULL, StartPostServer, NULL);

}

// Cleans up the UDP thread.
void stopUDP(void) {
//    pthread_join(PostMessage, NULL);
    pthread_join(UDP, NULL);

}

int socketDescriptor;

void* StartUDPServer(){
    // Address
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;                   // Connection may be from network
    sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
    sin.sin_port = htons(PORT);                 // Host to Network short

    // Create the socket for UDP
    socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // Bind the socket to the port (PORT) that we specify
    bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

    //error check
    if(!socketDescriptor){
        printf("UDP Server is unable to start\n");
        return NULL;
    }
    printf("UDP Server is ready!\n");

    while(isRunning){

        // Get the data (blocking)
        // Will change sin (the address) to be the address of the client.
        // Note: sin passes information in and out of call!
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);
        char messageRx[MSG_MAX_LEN];

        // Pass buffer size - 1 for max # bytes so room for the null (string data)
        int bytesRx = recvfrom(socketDescriptor,
                               messageRx, MSG_MAX_LEN - 1, 0,
                               (struct sockaddr *) &sinRemote, &sin_len);

        // Check for errors (-1)

        // Make it null terminated (so string functions work)
        // - recvfrom given max size - 1, so there is always room for the null
        messageRx[bytesRx] = 0;
//        printf("Message received (%d bytes): %s\n", bytesRx, messageRx);

        // separate the command and store into an array
        char *temp;
        char *cmd[MSG_MAX_LEN];

        temp = strtok(messageRx," ");
        int count = 0;
        while (temp != NULL){
            cmd[count] = temp;
            temp = strtok(NULL," \n");
            count ++;
        }

        //message to transfer;
        char messageTx[MSG_MAX_LEN];

        // empty cmd received, store previous cmd into current cmd
        if(strcmp(cmd[0], "\n") == 0){
            sprintf(cmd[0], "%s",previousCmd);
        }

        else{
            // store current cmd to previous cmd
            sprintf(previousCmd, "%s\n", cmd[0]);
        }

        // help
        if(strcmp(cmd[0], "help") == 0){
            sprintf(previousCmd, "help");
            sprintf(messageTx, "Accepted command examples:\n"
                               "mode 0 -- change the drum-beat mode to 0\n"
                               "volume 10 -- change the volume to 10 \n"
                               "tempo 120 --  change the tempo to 120 \n"
                               "play 1 -- play the drum cc sound"
                               );

            sin_len = sizeof(sinRemote);
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
        }
        // mode
        else if(strcmp(cmd[0], "mode") == 0){
            int choice = atoi(cmd[1]);

            if (setBeatSetting(choice)){
                sprintf(messageTx, "mode %d", getBeatSetting());
            }
            else{
                sprintf(messageTx, "mode %d", getBeatSetting());
            }
            // Transmit a reply:
            sin_len = sizeof(sinRemote);
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
        }
            //volume
        else if(strcmp(cmd[0], "volume") == 0){
            int choice = atoi(cmd[1]);

            if (AudioMixer_setVolume(choice)){
                sprintf(messageTx, "volume %d", AudioMixer_getVolume());
            }
            else{
                sprintf(messageTx, "volume %d", AudioMixer_getVolume());
            }
            // Transmit a reply:
            sin_len = sizeof(sinRemote);
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
        }
            //tempo
        else if(strcmp(cmd[0], "tempo") == 0){
            int choice = atoi(cmd[1]);

            //get only
//            if (choice == -1){
//                sprintf(messageTx, "%d", getBPM());
//            }
            if(setBPM(choice)){
                sprintf(messageTx, "tempo %d", getBPM());
            }
            else{
                sprintf(messageTx, "tempo %d", getBPM());
            }
            // Transmit a reply:
            sin_len = sizeof(sinRemote);
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
        }
            //play a sound
        else if(strcmp(cmd[0], "play") == 0){
            int choice = atoi(cmd[1]);

            if(playASound(choice)){
                sprintf(messageTx, "play sound: %d:\n", choice);
            }
            else{
                sprintf(messageTx, "invalid selection, only 0 to %d is available:\n", MAX_SOUND_SET-1);
            }
            // Transmit a reply:
            sin_len = sizeof(sinRemote);
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
        }

        else if(strcmp(cmd[0], "system") == 0){
            // Transmit a reply:
            getSystemUpTime(messageTx);
            sin_len = sizeof(sinRemote);
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);

            sprintf(messageTx, "tempo %d", getBPM());
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);

            sprintf(messageTx, "volume %d", getBPM());
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
        }

        else if(strcmp(cmd[0], "stop") == 0){
            // Transmit a reply:
            sprintf(messageTx, "stopping the program:\n");
            sin_len = sizeof(sinRemote);
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
            isRunning = false;
        }
        //
        else{
            sprintf(messageTx, "Unknown command. Type 'help' for command list.\n");
            // Transmit a reply:
            sin_len = sizeof(sinRemote);
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
        }
    }
    // Close
    close(socketDescriptor);

    return 0;
}


void getSystemUpTime(char * messageTx){

    FILE *fp = fopen("/proc/uptime", "r");
    if (fp == NULL){
        printf("ERROR: can't open /proc/uptime!");
        return;
    }
    char buffer[MSG_MAX_LEN];
    fgets(buffer, MSG_MAX_LEN, fp);
    fclose(fp);

    int uptime = atoi(buffer);

//    printf("uptime checking...........%d \n",uptime);
    int hours = uptime / 3600;
    int minutes = (uptime / 60) % 60;
    int seconds = uptime % 60;
    sprintf(messageTx, "system %d hours, %d minutes, %d seconds\n", hours, minutes, seconds);
}