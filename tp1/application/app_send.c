
#include "./app_send.h"

int sendFile(char* file_path) {

    // file contents
    uint8_t file_data[MAX_FILE_SIZE_B];
    int file_fd;

    file_fd = open(file_path,O_RDONLY);

    if (file_fd == -1){
        perror("file does not exist");
        exit(1);
    }

    int read_status = read(file_fd, file_data, MB_TO_B(MAX_FILE_SIZE_MB));

    if (read_status == -1){
        printf("file reading error");
        exit(2);
    }

    // file size
    char number[50];
    
    char* control_packet_arguments[4];
    control_packet_arguments[0] = "0";
    sprintf(number,"%d", strlen(file_data));
    
    // TODO: remove this print
    //printf("file size: %s \n", number);
    
    control_packet_arguments[1] = number;
    control_packet_arguments[2] = "1";
    control_packet_arguments[3] = file_path;

    // TODO: remove this print
    /*
    for (int i = 0; i < 4; i++) {
        printf("arg%d : %s \n", i, control_packet_arguments[i]);
    }
    */

    int connection_status = sendControlPacket(START, control_packet_arguments, 4);
    
    printf("%d \n",connection_status);

    return 0;
}

int calculate_arguments_size(char* arguments[], int argument_cnt) {

    int result = 0;

    for (int i = 0; i < argument_cnt; i += 2) {

        uint8_t type = (uint8_t) atoi(arguments[i]);

        switch (type) {
            case FILE_SIZE:
                result += sizeof(int);
                break;
            
            case FILE_NAME:
                result += sizeof(char) * strlen(arguments[i + 1]);
                break;

            default:
                return -1;
        }
    }

    return result;
}

int buildControlPacket(uint8_t* packet, char* arguments[], int argument_cnt) {

    int currentIndex = 1;

    for (int i = 0; i < argument_cnt; i += 2) {

        uint8_t type = (uint8_t) atoi(arguments[i]);
        // TODO: remove this print
        printf("type : %d \n", type);
        packet[currentIndex] = type;
        currentIndex++;

        switch (type) {
            case FILE_SIZE:
            {
                packet[currentIndex] = sizeof(int);
                // TODO: remove this print
                printf("length : %d \n", packet[currentIndex]);
                currentIndex++;
                int value = atoi(arguments[i + 1]);
                memcpy(packet + currentIndex, &value, sizeof(int));
                // TODO: remove this print
                printf("value : %d \n", (int) packet[currentIndex]);
                currentIndex += sizeof(int);
                break;
            }
            case FILE_NAME: 
            {
                int length = sizeof(char) * strlen(arguments[i + 1]);
                packet[currentIndex] = length;
                // TODO: remove this print
                printf("length : %d \n", packet[currentIndex]);
                currentIndex++;
                strcpy((char*) (packet + currentIndex), arguments[i + 1]);
                // TODO: remove from here
                printf("value : ");
                for (int k = currentIndex; k < currentIndex + length; k++) {
                    printf("%c", packet[k]);
                }
                printf("\n");
                // to here
                currentIndex += length;
                break;
            }
            default:
                return -1;
        }
    }

    return 0;
}

int sendControlPacket(enum control_type type, char* arguments[], int argument_cnt) {

    if (argument_cnt % 2 != 0)
        return -1;

    int arguments_value_total_size = calculate_arguments_size(arguments, argument_cnt);

    // TODO: remove this print
    //printf("Total arguments size : %d \n", arguments_value_total_size);

    int control_packet_size = sizeof(uint8_t) + argument_cnt * sizeof(uint8_t) * 2 + arguments_value_total_size;
    uint8_t* control_packet = malloc(control_packet_size);

    buildControlPacket(control_packet, arguments, argument_cnt);

    control_packet[0] = (uint8_t) type;

    for (int i = 0; i < control_packet_size; i++) {

        printf("- %c\n", control_packet[i]);
    }
    

    return 0;
}