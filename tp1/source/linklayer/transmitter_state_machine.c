#include "transmitter_state_machine.h"
#include "ll_consts.h"
#include "../util/log.h"


static FILE* log_fp = NULL;

void tsm_stm_set_log_fp(FILE* fp){
  log_fp = fp;
}

void tsm_process_input(struct transmitter_state_machine* st_machine, uint8_t received_byte) {
	
    switch(st_machine->currentState){

      case T_STATE_START:

        if(received_byte == FLAG) {
          log_debug(log_fp,"STM: At START state --> At FLAG state");
          st_machine->currentState = T_STATE_FLAG_RCV;
          st_machine->frame[FLAG_START_INDEX] = received_byte;
        }
        return;

      case T_STATE_FLAG_RCV:

        if(received_byte == ADDR_RECEIV_RES || received_byte == ADDR_RECEIV_COMMAND) {
          log_debug(log_fp,"STM: At FLAG state --> At ADDRESS state");
          st_machine->currentState = T_STATE_A_RCV;
          st_machine->frame[ADDR_INDEX] = received_byte;
        }
        else if(received_byte == FLAG)
          return;
        else {
          st_machine->currentState = T_STATE_START;
          log_debug(log_fp,"STM: At FLAG state --> At START state");
        }

        return;

      case T_STATE_A_RCV:

        if(received_byte == CONTROL_UA || received_byte != CONTROL_UA){
          log_debug(log_fp,"STM: At ADDRESS state --> At CONTROL state");
          st_machine->currentState = T_STATE_C_RCV;
          st_machine->frame[CTRL_INDEX] = received_byte;
        }
        else if(received_byte == FLAG) {
          log_debug(log_fp,"STM: At ADDRESS state --> At FLAG state");
          st_machine->currentState = T_STATE_FLAG_RCV;
        }
        else {
          st_machine->currentState = T_STATE_START;
          log_debug(log_fp,"STM: At ADDRESS state --> At START state");
        }
        return;

      case T_STATE_C_RCV:

        if(received_byte == (st_machine->frame[CTRL_INDEX] ^ st_machine->frame[ADDR_INDEX])){
          log_debug(log_fp,"STM: At C state --> At BCC state");
          st_machine->currentState = T_STATE_BCC_OK;
          st_machine->frame[BCC1_INDEX] = received_byte;
        }
        else if(received_byte == FLAG) {
          st_machine->currentState = T_STATE_FLAG_RCV;
          log_debug(log_fp,"STM: At C state --> At FLAG state");
        }
        else {
          st_machine->currentState = T_STATE_START;
          log_debug(log_fp,"STM: At C state --> At START state");
        }
        return;

      case T_STATE_BCC_OK:

        if(received_byte == FLAG){
          log_debug(log_fp,"STM: At BCC state --> At STOP state");
          st_machine->currentState = T_STATE_STOP;
          st_machine->frame[FLAG_END_INDEX] = received_byte;
        }
        else {
          log_debug(log_fp,"STM: At BCC state --> At START state");
          st_machine->currentState = T_STATE_START;
        }
        return;

      case T_STATE_STOP:
        log_debug(log_fp,"STM: At STOP state");
        return;

    }
}
