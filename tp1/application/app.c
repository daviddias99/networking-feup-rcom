#include <stdbool.h>

#define PACKET_MAX_DATA_SIZE            100

#include "./app_send.h"
#include "./app_receive.h"

FILE* log_fp = NULL;

int get_file_path_from_user(char* path){

    printf("File path : ");
    fflush(stdout);
    fgets(path, 255, stdin);
    log_debug(log_fp,"APP_T: Read path(%s) from user", path);
    path[strlen(path) - 1] = '\0';

    return 0;
}

void init_logging(){

  log_fp = fopen("app_log.txt","w");
  app_snd_set_log_fp(log_fp);
  app_rcv_set_log_fp(log_fp);
}

int main(int argc, char const *argv[]) {

    init_logging();

    if (argc < 3) {
      printf("Usage:\n\t%s transmitter [port] [path] [packet_size]\n\t receiver [port]", argv[0]);
      exit(1);
    }

    bool role;
    int port;

    if (strcmp(argv[1],"receiver") == 0) {
        role = false;
    }
    else if (strcmp(argv[1],"transmitter") == 0)
        role = true;
    else {
        perror("Role not recognized. Exiting...");
        exit(2);
    }
    
    port = atoi(argv[1]);

    if (role)  { 
        int packet_size;
        char* path;
        
        if (argc != 4) {
            printf("Usage:\n\t%s transmitter [port] [path] [packet_size]\n", argv[0]);
            exit(-1);
        } 

        if ((packet_size = atoi(argv[3])) < 0) {
            printf("Usage:\n\t%s transmitter [port] [path] [packet_size]\n", argv[0]);
        }

        path = malloc(strlen(argv[2]) * sizeof(char));
        path = strcpy(path, argv[2]);
        send_file(port, path, packet_size);
        free(path);
    }
    else {
        receive_file(port);
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
            printf(" %x", packet[i + j]);
        }
        printf("\n");
        i += length;
    }
}

void log_data_packet(uint8_t* packet) {
    printf("control : %d\n", packet[0]);
    printf("sequence number : %d\n", packet[1]);

    size_t length = 256 * packet[2] + packet[3];
    printf("size : 256 * %d + %d = %ld\n", packet[2], packet[3], length);

    printf("data :");
    for (size_t i = 0; i < length; i++)
        printf(" %x", packet[4 + i]);
    printf("\n");
}
