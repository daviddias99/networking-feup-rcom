#ifndef APP_SEND_H
#define APP_SEND_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "app_consts.h"


typedef struct tl_package {

    uint8_t type;
    uint8_t length;
} tl_package;

enum control_type {
    START = 2,
    END
};

int sendFile(char* file_path);
int sendControlPacket(enum control_type type, char* arguments[], int argument_cnt);

#endif