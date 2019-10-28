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
  receiver_state_t currentState;
  uint8_t* frame;
  size_t allocatedMemory;
  int currentByte_idx;
  bool connectionEstablished;
};

struct receiver_state_machine* create_rcv_state_machine();
void reset_rcv_state_machine(struct receiver_state_machine* st_machine);
void destroy_rcv_state_machine(struct receiver_state_machine* st_machine);
void sm_processInput(struct receiver_state_machine *st_machine, uint8_t received_byte);
void rcv_stm_set_log_fp(FILE* fp);

#endif