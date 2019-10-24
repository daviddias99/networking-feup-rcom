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


int send_file(char* file_path);
int build_start_packet(uint8_t* packet, tlv* tlv_list[]);

#endif