#ifndef APP_UTIL_H
#define APP_UTIL_H


#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"


/**
 * @brief Packet type
 */
typedef enum packet_type {
    DATA = 1,
    START,
    END
} packet_type;

/**
 * @brief Logs all the information present in a control packet: the type of the 
 * packet and each parameter's info (type, length, value).
 * 
 * @param packet        Pointer to a control packet
 * @param packet_size   Size of the packet
 */
void log_control_packet(uint8_t* packet, uint8_t packet_size);

/**
 * @brief Logs the information present in a data packet: the control byte, 
 * the sequence number, the length and the data itself.
 * 
 * @param packet        Pointer a data packet 
 */
void log_data_packet(uint8_t* packet);


/**
 * @brief Display a progress bar on the command line
 * 
 * @param prefix        String that is displayed before the progress bar
 * @param count         Progress
 * @param max_count     Maximum value that count can reach
 */
void progress_bar(const char* prefix, size_t count, size_t max_count);

#endif