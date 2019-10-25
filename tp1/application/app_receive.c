#include <string.h>

#include "./app_receive.h"
#include "./app.h"
#include "./tlv.h"


int receive_file(int port) {

    int port_fd = llopen(port, RECEIVER);
    if (port_fd < 0) {
        printf("No communication established\n");
        exit(1);
    }

    uint8_t packet[512];
    llread(port_fd, packet);
    
    if (packet[0] != START) {
        printf("Expected start packet\n");
        exit(1);
    }

    control_info* start_info = create_control_info(packet);

    int file_fd = open(start_info->file_name, O_CREAT | O_WRONLY);
    if (file_fd == -1) {
        perror("Unable to create file\n");
        exit(1);
    }


    int bytes_read;
    while ((bytes_read = llread(file_fd, packet)) > 0) {

        if (packet[0] == END) {
            break;
        }
        else if (packet[0] != DATA) {
            printf("Found unexpected type %d\n", packet[0]);
            exit(1);
        }

        write(file_fd, packet + 4, bytes_read - 4);
    }

    if (bytes_read < 0) {
        printf("Error while reading\n");
        exit(1);
    } 

    control_info* end_info = create_control_info(packet);

    // Check if start and end packets carried the same information
    if (compare_control_info(start_info, end_info) == 0) {
        printf("Start and end packets carried different information\n");
    }

    // TODO: check if the written file as the same size as the one on the control packets

    destroy_control_info(start_info);
    destroy_control_info(end_info);

    close(file_fd);
    llclose(port_fd);
}

control_info* create_control_info(uint8_t* packet) {
    if (packet[0] != END && packet[0] != START) {
        printf("Unexpected packet type\n");
        return NULL;
    }

    control_info* info = malloc(sizeof(control_info));
    if (info == NULL) {
        printf("Unable to allocate memory\n");
        return NULL;
    }

    size_t packet_index = 1;    

    while (1) {
        int type = packet[packet_index];
        packet_index++;
        uint8_t length = packet[packet_index];
        packet_index++;

        switch (type) {
            case FILE_SIZE:
            {
                info->file_size = array_to_int(packet + packet_index);
                break;
            }
            case FILE_NAME:
            {  
                info->file_name = strndup(packet + packet_index, length);
                if (info->file_name == NULL) {
                    printf("Error allocating memory for file name\n");
                    return NULL;
                }
                break;
            }
            default:
            {
                printf("Found unexpected parameter type\n");
                return NULL;
            }
        }
        
        packet_index += length;
    }
    return info;
}

void destroy_control_info(control_info* info) {
    free(info);
}

int compare_control_info(control_info* start_info, control_info* end_info) {
    if (start_info->file_size != end_info->file_size)
        return 0;
    if (strcmp(start_info->file_name, end_info->file_name) != 0)
        return 0;

    return 1;
}