#ifndef TRANSMITER_STM_H
#define TRANSMITER_STM_H

#include <stdint.h>
#include <stdio.h>

typedef enum transmitter_state {
                      T_STATE_START = 0,
                      T_STATE_FLAG_RCV,
                      T_STATE_A_RCV,
                      T_STATE_C_RCV,
                      T_STATE_BCC_OK,
                      T_STATE_STOP
                   } trasmiter_state_t;

struct transmitter_state_machine {
    enum transmitter_state currentState;
    uint8_t frame[5];
};

void tsm_process_input(struct transmitter_state_machine* st_machine, uint8_t received_byte);
void tsm_stm_set_log_fp(FILE* fp);

#endif
