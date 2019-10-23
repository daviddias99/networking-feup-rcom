#include <stdbool.h>

#define FALSE 0
#define TRUE 1


#define FILE_PATH                       "/home/david/Documents/Repositories/RCOM-Projects/Trabalho_1/TP_1/application/teste.txt"
#define PACKET_MAX_DATA_SIZE            256

#include "./app_send.h"


int main(int argc, char const *argv[])
{
    if(argc != 2){
      printf("Usage:\tnserial Role\n\tex: serial_transfer receiver\n");
      exit(1);
    }

    int role;
    
    if(strcmp(argv[1],"receiver") == 0)
        role = FALSE;
    else if (strcmp(argv[1],"transmitter") == 0)
        role = TRUE;
    else
    {
        perror("Role not recognized. Exiting...");
        exit(2);
    }
    
    if(role){
        sendFile(FILE_PATH);
    }else{
        // receiveFile();
    }



    return 0;
}
