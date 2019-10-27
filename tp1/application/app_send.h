#ifndef APP_SEND_H
#define APP_SEND_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "./app.h"
#include "./tlv.h"
#include "../linklayer/linklayer.h"


// TODO: change this value
// this macro defines the number of bytes of data present in each packet
#define MAX_PACKET_DATA             200
#define PACKET_SIZE                 (4 + MAX_PACKET_DATA)


int send_file(char* file_path);
uint8_t* build_control_packet(packet_type type, uint8_t* packet_size, tlv* tlv_list[], const uint8_t tlv_list_size);
uint8_t* build_data_packet(uint8_t* data, uint8_t data_size);

char* name_from_path(char* path);

void app_snd_set_log_fp(FILE* fp);

#endif