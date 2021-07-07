#ifndef APP_SEND_H
#define APP_SEND_H

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "./app_util.h"
#include "./tlv.h"
#include "../linklayer/linklayer.h"


/**
 * @brief Sends a file to the receiver
 * 
 * @param port          Port that will be used for the communication (ex: port = 1 opens /dev/ttyS1)
 * @param file_path     Path of the file that will be transfered
 * @param packet_size   Maximum size of each packet
 * 
 * @return int          Returns 1 in case of error, otherwise returns 0
 */
int send_file(int port, char* file_path, size_t packet_size);

/**
 * @brief Builds a control packet with the provided parameters. This function allocates memory to store the 
 * the packet and returns a pointer to it.
 * 
 * @param type              Control packet type
 * @param packet_size       Memory address where the packet size will be stored
 * @param tlv_list          Parameter list
 * @param tlv_list_size     Size of the parameter list
 * 
 * @return uint8_t*         Returns a pointer to the packet, returns NULL in case of error
 */
uint8_t* build_control_packet(packet_type type, int* packet_size, tlv* tlv_list[], const uint8_t tlv_list_size);

/**
 * @brief Builds a data packet with the provided data. This function allocates
 * 
 * @param data              Pointer to the data
 * @param data_size         Data size
 * 
 * @return uint8_t*         Returns a pointer to the packet, returns NULL in case of error
 */
uint8_t* build_data_packet(uint8_t* data, size_t data_size);

/**
 * @brief Get the file name from a file path
 * 
 * @param path              File path
 * 
 * @return char*            Returns the file name
 */
char* name_from_path(char* path);

/**
 * @brief Sets the file pointer where the execution of the app will be logged
 * 
 * @param fp                Pointer to the log file
 */
void app_snd_set_log_fp(FILE* fp);

#endif
