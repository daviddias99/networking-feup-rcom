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
#include <sys/times.h>
#include "../util/log.h"

#define RECEIVER    0               // serial port receiver-id
#define TRANSMITTER 1               // serial port transmitter-id

/**
 * @brief Establishes the serial port connection between a receiver and a transmitter.
 * The function waits for the other party(with the opposing role) to also open the conncection.
 * @param port          Number of the serial port where the connection must be established (ex: port=1 opens dev/ttyS1)
 * @param role          0 for receiver, 1 for transmitter
 * @return int          0 upon sucess, non-0 otherwise
 */
int llopen(int port, int role);

/**
 * @brief Writes a byte buffer with a given length to the openned port(given in fd parameter).
 * 
 * @param fd            file descriptor of the serial port
 * @param buffer        byte buffer to be transmitted
 * @param length        number of bytes of the buffer to transmitt
 * @return int          number of bytes written or -1 upon error (not oppening the connection previously for example)
 */
int llwrite(int fd, uint8_t * buffer, int length);

/**
 * @brief Reads bytes from a given byte buffer
 * 
 * @param fd            file descriptor of the serial port
 * @param buffer        byte buffer where the read bytes will be stored
 * @return int          number of bytes read or -1 upon error (not oppening the connection previously for example). Returns 0 if port closed after establishing.
 */
int llread(int fd, uint8_t * buffer);

/**
 * @brief Closes the serial port communication channel
 * 
 * @param fd            channel to close
 * @return int          0 upon sucess, non-0 otherwise
 */
int llclose(int fd);

#endif
