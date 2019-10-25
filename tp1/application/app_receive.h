#ifndef APP_RECEIVE_H
#define APP_RECEIVE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>



typedef struct control_info_st {
    char* file_name;
    int file_size;
} control_info;

control_info* create_control_info(uint8_t* packet, const size_t packet_size);
void destroy_control_info(control_info* info);
int compare_control_info(control_info* start_info, control_info* end_info);




int receive_file(int port);



#endif
