#include <stdbool.h>

#include "./app_send.h"
#include "./app_receive.h"


/**
 * @brief Pointer to the log file
 * 
 */
FILE* log_fp = NULL;

/**
 * @brief Initiates the files needed for app layer logging purposes (does the same for app layer segments)
 * 
 */
void init_logging(){
  log_fp = fopen("app_log.txt","w");
  app_snd_set_log_fp(log_fp);
  app_rcv_set_log_fp(log_fp);
}

int main(int argc, char const *argv[]) {

    // Init the file used for the app layer logging (this call is part of the logging process and does not affect the program)
    init_logging();

    if (argc < 3) {
      printf("Usage:\t%s transmitter [port] [path] [packet_size]\n\t%s receiver [port]\n\n", argv[0], argv[0]);
      exit(1);
    }

    bool role;          // false if receiver, true if transmitter
    int port;           // serial port (ex: port=1 opens /dev/ttyS1)

    if (strcmp(argv[1],"receiver") == 0) {
        role = false;
    }
    else if (strcmp(argv[1],"transmitter") == 0)
        role = true;
    else {
        perror("Role not recognized. Exiting...");
        exit(2);
    }
    
    // Extract user arguments and call the correct function

    port = atoi(argv[1]);

    if (role)  { 
        int packet_size;
        char* path;
        
        
        if (argc != 5) {
            printf("Usage:\t%s transmitter [port] [path] [packet_size]\n", argv[0]);
            exit(-1);
        } 

        if ((packet_size = atoi(argv[4])) < 0) {
            printf("Usage:\t%s transmitter [port] [path] [packet_size]\n", argv[0]);
        }

        path = malloc(strlen(argv[3]) * sizeof(char));
        path = strcpy(path, argv[3]);

        // send file to receiver
        send_file(port, path, packet_size);
        free(path);
    }
    else {

        // receive file from transmtiter
        receive_file(port);
    }

    return 0;
}

