#ifndef APP_RECEIVE_H
#define APP_RECEIVE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "./app_util.h"
#include "./tlv.h"
#include "../linklayer/linklayer.h"

/**
 * @brief control_info object that stores the file information
 */
typedef struct control_info_st {
    char* file_name;    // file name
    int file_size;      // file size
} control_info;

/**
 * @brief Create a control_info object by extracting the information from a control packet. 
 * This function allocates memory to store the object.
 * 
 * @param packet            Control packet from which the information will be extracted
 * @param packet_size       Size of the control packet
 * 
 * @return control_info*    Returns a pointer to the control_info, returns NULL in case of error
 */
control_info* create_control_info(uint8_t* packet, const size_t packet_size);

/**
 * @brief Destroy a control_info object. This function frees the memory occupied by the object.
 * 
 * @param info              Pointer a control_info object
 */
void destroy_control_info(control_info* info);

/**
 * @brief Compares two control_info objects.
 * 
 * @param start_info    Information extracted from the start packet
 * @param end_info      Inforamtion extracted from the end packet
 * 
 * @return int          Returns 0 if the information is different, returns 1 if it is equal
 */
int compare_control_info(control_info* start_info, control_info* end_info);

/**
 * @brief Receives a file from the transmitter
 * 
 * @param port 
 * @return int 
 */
int receive_file(int port);


/**
 * @brief Sets the file pointer where the execution of the app will be logged
 * 
 * @param fp                Pointer to the log file
 */
void app_rcv_set_log_fp(FILE* fp);



#endif
