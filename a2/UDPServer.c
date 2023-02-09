
#include "UDPServer.h"
#include "LightSensorA2D.h"
#include "HistoryArrayAlloc.h"
#include "Global.h"

#include <stdio.h>
#include <string.h>

void* StartUDP()
{
    // Address
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;                   // Connection may be from network
    sin.sin_addr.s_addr = htonl(INADDR_ANY);    // Host to Network long
    sin.sin_port = htons(PORT);                 // Host to Network short

    // Create the socket for UDP
    int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0);

    // Bind the socket to the port (PORT) that we specify
    bind (socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

    // Check for errors (-1)

    while (1) {
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
        char *temp;
        char *cmd[MSG_MAX_LEN];

        temp = strtok(messageRx," ");
        int count = 0;
        while (temp != NULL){
            cmd[count] = temp;
            temp = strtok(NULL," \n");
            count ++;
        }


        char messageTx[MSG_MAX_LEN];
        char currentCmd[MSG_MAX_LEN];

        int cmdNumber = 0;

        if(strcmp(cmd[0], "\n") == 0){
            sprintf(cmd[0], "%s",currentCmd);
        }
        char message[MSG_MAX_LEN] = "";
        if(strcmp(cmd[0], "help\n") == 0){
            sprintf(currentCmd, "help\n");
            sprintf(messageTx, "Accepted command examples:\n"
                               "count -- display total number of samples taken.\n"
                               "length -- display number of samples in history (both max, and current).\n"
                               "history -- display the full sample history being saved.\n"
                               "get 10 -- display the 10 most recent history values.\n"
                               "dips -- display number of .\n"
                               "stop -- cause the server program to end.\n"
                               "<enter> -- repeat last command.\n");

            sin_len = sizeof(sinRemote);
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);

        }
        else if(strcmp(cmd[0], "count\n") == 0){
            sprintf(currentCmd, "count\n");
            sprintf(messageTx, "Number of samples taken = %d\n", getNumOfSamples());

            // Transmit a reply:
            sin_len = sizeof(sinRemote);
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
        }
        else if(strcmp(cmd[0], "length\n") == 0){
            sprintf(currentCmd, "length\n");
            int validSize = getNumOfSamples();
            if(validSize > GetCurrentSampleSize()){
                validSize = GetCurrentSampleSize();
            }
            sprintf(messageTx, "History can hold %d samples.\n"
                               "Currently holding %d samples.\n", GetCurrentSampleSize(), GetHistorySize());

            // Transmit a reply:
            sin_len = sizeof(sinRemote);
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
        }
        else if(strcmp(cmd[0], "history\n") == 0){
            sprintf(currentCmd, "history\n");

            int* array = GetHistoryArray();

            char buffer[MSG_MAX_LEN];
            for (int i = 1 ;i < GetCurrentSampleSize(); i++){
                sprintf(buffer, "%0.3f, ", ComputeVoltage(array[i]));
                strcat(message, buffer);

                if((i-1)%20 == 19){
                    sprintf(buffer, "\n");
                    strcat(message, buffer);
                    sendto( socketDescriptor,
                            message, strlen(message),
                            0,
                            (struct sockaddr *) &sinRemote, sin_len);
                    message[0] = '\0';
                }
            }
            sprintf(buffer, "\n");
            strcat(message, buffer);
            sendto( socketDescriptor,
                    message, strlen(message),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
            message[0] = '\0';
        }


        else if(strcmp(cmd[0], "get") == 0){
            sprintf(currentCmd, "get");

            cmdNumber = atoi(cmd[1]);
            printf("%d", cmdNumber);

            if(cmdNumber >= GetCurrentSampleSize()){
                cmdNumber = GetCurrentSampleSize() - 1;
            }

            int* array = GetHistoryArray();
            char buffer[MSG_MAX_LEN];
            for (int i = 1 ;i < cmdNumber + 1; i++){

                sprintf(buffer, "%0.3f, ", ComputeVoltage(array[i]));
                strcat(message, buffer);

                if((i-1)%20 == 19){
                    sprintf(buffer, "\n");
                    strcat(message, buffer);
                    sendto( socketDescriptor,
                            message, strlen(message),
                            0,
                            (struct sockaddr *) &sinRemote, sin_len);
                    message[0] = '\0';
                }
            }
            sprintf(buffer, "\n");
            strcat(message, buffer);
            sendto( socketDescriptor,
                    message, strlen(message),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
            message[0] = '\0';
        }

        else if(strcmp(cmd[0], "dips\n") == 0){
            sprintf(currentCmd, "dips\n");
            sprintf(messageTx, "Dips = %d\n", GetNumOfDips());

            // Transmit a reply:
            sin_len = sizeof(sinRemote);
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
        }

        else if(strcmp(cmd[0], "stop\n") == 0){

            sprintf(messageTx, "Program terminating.\n");
            sin_len = sizeof(sinRemote);
            isRunning = false;
            sendto( socketDescriptor,
                    messageTx, strlen(messageTx),
                    0,
                    (struct sockaddr *) &sinRemote, sin_len);
            break;
        }
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
