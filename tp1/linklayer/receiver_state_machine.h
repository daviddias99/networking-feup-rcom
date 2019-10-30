#ifndef RCV_STATE_MACHINE_H
#define RCV_STATE_MACHINE_H

#include "ll_consts.h"
#include "../util/log.h"
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

// Frame receiving state-machine states
typedef enum receiver_state {
  R_STATE_START = 0,
  R_STATE_FLAG_RCV,
  R_STATE_A_RCV,
  R_STATE_SU_C_RCV,
  R_STATE_SU_BCC1_OK,
  R_STATE_I_C_RCV,
  R_STATE_I_DATA_RCV,
  R_STATE_ESCAPE_FOUND,
  R_STATE_SU_STOP,
  R_STATE_I_STOP,
} receiver_state_t;

// State-machine representing the receiving of type S or U
struct receiver_state_machine {
  receiver_state_t currentState;    // current state of the machine
  uint8_t* frame;                   // frame storage
  size_t allocatedMemory;           // bytes allocated to the frame storage
  size_t currentByte_idx;           // current index of the place where the next received byte shall be written
  bool connectionEstablished;       // true if the connection as been established, false otherwise
};


/**
 * @brief Allocated necessary memmory and initializes the fields of a new state machine
 * 
 * @return struct receiver_state_machine* pointer to a new state machine
 */
struct receiver_state_machine* create_rcv_state_machine();

/**
 * @brief Resets the values of the state machine
 * 
 * @param st_machine  state machine to reset
 */
void reset_rcv_state_machine(struct receiver_state_machine* st_machine);

/**
 * @brief Frees allocated memory for the state machine
 * 
 * @param st_machine  state machine to destroy
 */
void destroy_rcv_state_machine(struct receiver_state_machine* st_machine);

/**
 * @brief Processes a byte. Alters the internal state of the STM
 * 
 * @param st_machine            state-machine that processed the byte
 * @param received_byte         byte to process
 */
void sm_processInput(struct receiver_state_machine *st_machine, uint8_t received_byte);

/**
 * @brief Sets the logging file for the state machine
 * 
 * @param fp                    file pointer of the log file
 */ 
void rcv_stm_set_log_fp(FILE* fp);

#endif