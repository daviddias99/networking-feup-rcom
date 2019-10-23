
#include "./app_send.h"


int sendFile(char* file_path){

    uint8_t file_data[MB_TO_B(MAX_FILE_SIZE_MB)];
    int file_fd;

    file_fd = open(file_path,O_RDONLY);

    if(file_fd == -1){
        perror("file does not exist");
        exit(1);
    }

    int read_status = read(file_fd,file_data,MB_TO_B(MAX_FILE_SIZE_MB));

    if(read_status == -1){
        printf("file reading error");
        exit(2);
    }

    char number[50];
    
    char* control_packet_arguments[4];
    control_packet_arguments[0] = "0";
    sprintf(number,"%d",(int)MB_TO_B(MAX_FILE_SIZE_MB));
    control_packet_arguments[1] = number;
    control_packet_arguments[2] = "1";
    control_packet_arguments[3] = file_path;

    int connection_status = sendControlPacket(START,control_packet_arguments,4);
    
    printf("%d \n",connection_status);

    return 0;
}

int calculate_arguments_size(char* arguments[],int argument_cnt){

    int result = 0;

    for(int i = 0; i < argument_cnt; i+=2){

        if(strcmp(arguments[i],"0") == 0){
            result += sizeof(int);
        }
        else if(strcmp(arguments[i],"1") == 0){
            result += sizeof(char) * strlen(arguments[i+1]);
        }
        else{
            return -1;
        }
    }

    return result;
}

int buildControlPacket(uint8_t* packet,char* arguments[], int argument_cnt){

    int currentIndex = 1;

    for(int i = 0; i < argument_cnt;i+=2){

        uint8_t type = (uint8_t) atoi(arguments[i]);
        packet[currentIndex] = type;
        currentIndex++;
        
        if(type == 0){

            packet[currentIndex] = sizeof(int);
            currentIndex++;
            int value =  atoi(arguments[i+1]);
            memcpy(packet+currentIndex,&value,sizeof(int));
            currentIndex+=sizeof(int);

        }
        else if(type == 1){
            int length = sizeof(char) * strlen(arguments[i+1]);
            packet[currentIndex] = length;
            currentIndex++;
            strcpy((char*)(packet+currentIndex),arguments[i+1]);
            currentIndex+=length;
        }
        else{

            return -1;
        }

    }

    return 0;
}

int sendControlPacket(enum control_type type, char* arguments[], int argument_cnt){

    if(argument_cnt % 2 != 0)
        return -1;

    int arguments_value_total_size = calculate_arguments_size(arguments, argument_cnt);
    int control_packet_size = sizeof(uint8_t) + argument_cnt * sizeof(uint8_t)*2 + arguments_value_total_size;
    uint8_t* control_packet = malloc(control_packet_size);

    buildControlPacket(control_packet,arguments,argument_cnt);

    control_packet[0] = (uint8_t) type;

    for(int i = 0; i < control_packet_size;i++){

        printf("- %c\n", control_packet[i]);
    }
    

    return 0;
}