#include <stdbool.h>

#define FALSE 0
#define TRUE 1

#define PACKET_MAX_DATA_SIZE            256

#include "./app_send.h"


int main(int argc, char const *argv[])
{
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
    
    if (role){
        /*
        printf("File path : ");
        fflush(stdout);
        */
        char buffer[255];
        /*
        read(STDIN_FILENO, buffer, 255);
        buffer[strlen(buffer) - 1] = '\0';
        */
        // TODO: uncomment code above and remove sprintf
        sprintf(buffer, "resources/test.md");
        send_file(buffer);
    }
    else {
        // receiveFile();
    }

    return 0;
}
