#include <stdbool.h>

#define FALSE 0
#define TRUE 1

#define PACKET_MAX_DATA_SIZE            256

#include "./app_send.h"
#include "./app_receive.h"


int main(int argc, char const *argv[]) {
    if (argc != 2) {
      printf("Usage:\tnserial Role\n\tex: serial_transfer receiver\n");
      exit(1);
    }

    int role;
    
    if (strcmp(argv[1],"receiver") == 0)
        role = FALSE;
    else if (strcmp(argv[1],"transmitter") == 0)
        role = TRUE;
    else {
        perror("Role not recognized. Exiting...");
        exit(2);
    }
    char path[255];
    
    if (role){
        printf("File path : ");
        fflush(stdout);    
        fgets(path, 255, stdin);
        printf("path after read %s\n", path);
        path[strlen(path) - 1] = '\0';
        send_file(path);
    }
    else {
        receive_file(0);
    }

    return 0;
}

void log_control_packet(uint8_t* packet, uint8_t packet_size) {
    printf("type: %d\n", packet[0]);
    
    uint8_t i = 1;
    uint8_t parameter_number = 0;
    while (i < packet_size) {
        printf("parameter number : %d\n", parameter_number);
        printf("    type : %d\n", packet[i]);
        i++;
        uint8_t length = packet[i];
        printf("    length : %d\n", length);
        i++;
        printf("    value :");
        for (uint8_t j = 0; j < length; j++) {
            printf(" %d", packet[i + j]);
        }
        printf("\n");
        i += length;
    }
}

void log_data_packet(uint8_t* packet) {
    printf("control : %d\n", packet[0]);
    printf("sequence number : %d\n", packet[1]);

    size_t length = 256 * packet[2] + packet[3];
    printf("size : 256 * %d + %d = %d\n", packet[2], packet[3], length);
    
    printf("data :");
    for (size_t i = 0; i < length; i++)
        printf(" %d", packet[4 + i]);
    printf("\n");
}
