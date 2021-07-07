#ifndef TLV_H
#define TLV_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


/**
 * @brief Data type
 * 
 */
typedef enum data_type {
    FILE_SIZE = 0,
    FILE_NAME
} data_type;


/**
 * @brief Value type
 * 
 */
typedef enum value_type {
    STRING = 0,
    INT
} value_type;


/**
 * @brief tlv object, stores the type, length and value of an int or string
 * 
 */
typedef struct tlv_package {
    data_type type;         // data type
    uint8_t length;         // length of the value in bytes
    uint8_t* value;         // array where the value is stored
    value_type value_type;  // type of the value (integer or string)
} tlv;

/**
 * @brief Create a tlv object to store an integer value. This function allocates 
 * memory to store the object.
 * 
 * @param type      Data type
 * @param value     Integer value
 * 
 * @return tlv*     Returns a pointer to the tlv object, returns NULL in case of error
 */
tlv* create_tlv_int(data_type type, int value);

/**
 * @brief Create a tlv object to store a string. This function allocates memory 
 * to store the object.
 * 
 * @param type      Data type
 * @param value     String
 * 
 * @return tlv*     Returns a pointer to the tlv object, returns NULL in case of error
 */
tlv* create_tlv_str(data_type type, char* value);


/**
 * @brief Destroy the tlv object. This function frees the memory occupied by the 
 * object.
 * 
 * @param tlv_ptr   Pointer to the tlv object that will be destroyed
 */
void destroy_tlv(tlv* tlv_ptr);


/**
 * @brief Prints the information contained in the tlv object to the command line.
 * 
 * @param tlv_ptr   Pointer to the tlv object that contains the information
 */
void print_tlv(tlv* tlv_ptr);

/**
 * @brief Turns an [int] value to an array of [uint8_t]
 * 
 * @param integer   Integer value
 * @param array     Pointer to the array
 */
void int_to_array(int integer, uint8_t* array);

/**
 * @brief Turns an array of [uint8_t] to an [int] value
 * 
 * @param array     Pointer to the array
 * @return int      Returns the integer value
 */
int array_to_int(uint8_t* array);

#endif
