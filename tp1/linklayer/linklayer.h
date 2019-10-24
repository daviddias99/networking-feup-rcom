#ifndef LINK_LAYER_H
#define LINK_LAYER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <signal.h>
#include "../util/log.h"

#define RECEIVER    0
#define TRANSMITTER 1

int llopen(int port, int role);
int llwrite(int fd, char * buffer, int length);
int llread(int fd, char * buffer);
int llclose(int fd);

#endif
