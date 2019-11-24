#include "receiver_state_machine.h"

static FILE* log_fp = NULL;

void rcv_stm_set_log_fp(FILE* fp){
  log_fp = fp;
}

struct receiver_state_machine* create_rcv_state_machine() {
  struct receiver_state_machine * st_machine = malloc(sizeof(struct receiver_state_machine));
  st_machine->currentState = R_STATE_START;
  st_machine->connectionEstablished = false;
  st_machine->currentByte_idx = 0;
  st_machine->allocatedMemory = I_FRAME_START_SIZE;
  st_machine->frame = malloc(sizeof(uint8_t) * st_machine->allocatedMemory);

  return st_machine;
}

void reset_rcv_state_machine(struct receiver_state_machine* st_machine) {
  st_machine->currentState = R_STATE_START;
  st_machine->currentByte_idx = 0;
}

void destroy_rcv_state_machine(struct receiver_state_machine* st_machine) {
  free(st_machine->frame);
  free(st_machine);
}


void sm_start_st_handler(struct receiver_state_machine *st_machine, uint8_t receivedByte)
{

  if (receivedByte == FLAG)
  {
    st_machine->currentState = R_STATE_FLAG_RCV;
    st_machine->frame[FLAG_START_INDEX] = receivedByte;
    log_debug(log_fp,"STM: At start state --> At flag state");
    return;
  }

  log_debug(log_fp,"STM: At start state --> At start state");

  return;
}

void sm_flag_st_handler(struct receiver_state_machine *st_machine, uint8_t receivedByte)
{

  if (receivedByte == ADDR_TRANSM_COMMAND || receivedByte == ADDR_TRANSM_RES)
  {
    st_machine->currentState = R_STATE_A_RCV;
    st_machine->frame[ADDR_INDEX] = receivedByte;
    log_debug(log_fp,"STM: At flag state --> At A state");
  }
  else if (receivedByte == FLAG){

    log_debug(log_fp,"STM: At flag state --> At flag state");
  }
  else{

    st_machine->currentState = R_STATE_START;
    log_debug(log_fp,"STM: At flag state --> At start state");
  }
  
  return;
}

void sm_a_rcv_st_handler(struct receiver_state_machine *st_machine, uint8_t receivedByte)
{ 
  // log_debug(log_fp,"\nat control, connection established %s\b", st_machine->connectionEstablished ? "TRUE" : "FALSE");

  if ((receivedByte == CONTROL_SET) || (receivedByte == CONTROL_DISC) || (receivedByte == CONTROL_UA))
  { // if the received command is to establish/disconnect a communication  or a UA(S or U)

    st_machine->currentState = R_STATE_SU_C_RCV;
    st_machine->frame[CTRL_INDEX] = receivedByte;
    st_machine->connectionEstablished = false;

    log_debug(log_fp,"STM: At A state --> At SU C RCV state");
  }
  else if (receivedByte == FLAG){

    st_machine->currentState = R_STATE_FLAG_RCV;
    log_debug(log_fp,"STM: At A state --> At flag state");
  }
  else if (((receivedByte == 0x00) || (receivedByte == 0x40)) && (st_machine->connectionEstablished))
  { // if the received command is to receive data (I)
    st_machine->frame[CTRL_INDEX] = receivedByte;
    st_machine->currentState = R_STATE_I_C_RCV;
    log_debug(log_fp,"STM: At A state --> At I C RCV state");
  }
  else{

    st_machine->currentState = R_STATE_START;
    log_debug(log_fp,"STM: At A state --> At start state");
  }
  
  return;
}

void sm_su_c_rcv_st_handler(struct receiver_state_machine *st_machine, uint8_t receivedByte)
{

  log_debug(log_fp,"STM: At SU C RCV state");
  if (receivedByte == (st_machine->frame[CTRL_INDEX] ^ st_machine->frame[ADDR_INDEX]))
  {
    st_machine->currentState = R_STATE_SU_BCC1_OK;
    st_machine->frame[BCC1_INDEX] = receivedByte;
  }
  else if (receivedByte == FLAG)
    st_machine->currentState = R_STATE_FLAG_RCV;
  else
    st_machine->currentState = R_STATE_START;

  return;
}

void sm_i_c_rcv_st_handler(struct receiver_state_machine *st_machine, uint8_t receivedByte)
{

  if (receivedByte == (st_machine->frame[CTRL_INDEX] ^ st_machine->frame[ADDR_INDEX]))
  { // check the first BCC
    st_machine->currentState = R_STATE_I_DATA_RCV;
    st_machine->frame[BCC1_INDEX] = receivedByte;
    st_machine->currentByte_idx = I_FRAME_DATA_START_INDEX;
    log_debug(log_fp,"STM: At I C RCV state --> At Data RCV state (correct header bcc)");
  }
  else if (receivedByte == FLAG){

    st_machine->currentState = R_STATE_FLAG_RCV;
    log_debug(log_fp,"STM: At I C RCV state --> At flag state");
  }
  else{

    st_machine->currentState = R_STATE_START;
    log_debug(log_fp,"STM: At I C RCV state --> At start state");
  }

  return;
}

void sm_i_data_rcv_st_handler(struct receiver_state_machine *st_machine, uint8_t receivedByte)
{ 

  // TODO: Change this limit
  if(st_machine->currentByte_idx == st_machine->allocatedMemory){

    st_machine->allocatedMemory *= 2;
    st_machine->frame = realloc(st_machine->frame, st_machine->allocatedMemory);
    log_debug(log_fp,"\n\n REALLOC MEMORY %ld\n\n", st_machine->allocatedMemory);
  }

  if (receivedByte == 0x7d)
  { // escape character must be converted to original chararacter

    st_machine->currentState = R_STATE_ESCAPE_FOUND;
    log_debug(log_fp,"STM: At Data RCV state --> At ESCAPE CODE Found state");
  }
  else if (receivedByte == FLAG)
  { // end of the frame. This means the previous byte was the BCC of the data and must be checked

    if(st_machine->currentByte_idx < 6){    // the end flag was received but no bcc and or data was received

      st_machine->currentState = R_STATE_START;
      log_debug(log_fp,"STM: At Data RCV state --> At start state (flag received, but no bcc or data)");
      return;
    }

    st_machine->frame[st_machine->currentByte_idx] = receivedByte;
    st_machine->currentByte_idx++;
    st_machine->currentState = R_STATE_I_STOP;
    log_debug(log_fp,"STM: At Data RCV state --> At I stop state");
  }
  else
  {
    st_machine->frame[st_machine->currentByte_idx] = receivedByte; // normal data byte
    st_machine->currentByte_idx++;
    log_debug(log_fp,"STM: At Data RCV state --> At Data RCV state");
  }

  return;
}

void sm_esc_found_st_handler(struct receiver_state_machine *st_machine, uint8_t receivedByte)
{


  if (receivedByte == 0x5d)
  {

    st_machine->frame[st_machine->currentByte_idx] = 0x7d; // parse escape byte
    st_machine->currentByte_idx++;
    st_machine->currentState = R_STATE_I_DATA_RCV;

    log_debug(log_fp,"STM: At ESCAPE Code Found state --> At Data Receive State (escape code parsed)");
  }
  else if (receivedByte == 0x5e)
  {
    st_machine->frame[st_machine->currentByte_idx] = 0x7e; // parse flag byte
    st_machine->currentByte_idx++;
    st_machine->currentState = R_STATE_I_DATA_RCV;
    log_debug(log_fp,"STM: At ESCAPE Code Found state --> At Data Receive State (flag parsed)");
  }
  else
  {
    st_machine->currentState = R_STATE_START;
    log_debug(log_fp,"STM: At ESCAPE Code Found state --> At Start state");
  }

  return;
}

void sm_su_bcc1_st_handler(struct receiver_state_machine *st_machine, uint8_t receivedByte)
{

  
  if (receivedByte == FLAG)
  {

    st_machine->currentState = R_STATE_SU_STOP;
    st_machine->frame[FLAG_END_INDEX] = receivedByte;
    st_machine->connectionEstablished = true;
    log_debug(log_fp,"STM: At SU_bcc1 state --> At SU Stop state");
  }
  else{
    st_machine->currentState = R_STATE_START;
    log_debug(log_fp,"STM: At SU_bcc1 state --> At start state");
  }

  return;
}

void sm_su_state_stop_st_handler(struct receiver_state_machine *st_machine, uint8_t receivedByte)
{


  if (receivedByte == FLAG)
  {
    st_machine->currentState = R_STATE_FLAG_RCV;
    st_machine->frame[FLAG_END_INDEX] = receivedByte;
    log_debug(log_fp,"STM: At SU stop state --> At flag state");
  }
  else{

    st_machine->currentState = R_STATE_START;
    log_debug(log_fp,"STM: At SU stop state --> At start state ");
  }


  return;
}

void sm_i_state_stop_st_handler(struct receiver_state_machine *st_machine, uint8_t receivedByte)
{

  
  if (receivedByte == FLAG)
  {

    st_machine->currentState = R_STATE_FLAG_RCV;
    st_machine->frame[FLAG_END_INDEX] = receivedByte;

    log_debug(log_fp,"STM: At I stop state --> At flag state");
  }
  else{

    st_machine->currentState = R_STATE_START;
    log_debug(log_fp,"STM: AtI stop state --> At start state");
  }
    
  return;
}

static void (*rcv_event_handlers[])(struct receiver_state_machine *, uint8_t) = {
                                                              sm_start_st_handler,
                                                              sm_flag_st_handler,
                                                              sm_a_rcv_st_handler,
                                                              sm_su_c_rcv_st_handler,
                                                              sm_su_bcc1_st_handler,
                                                              sm_i_c_rcv_st_handler,
                                                              sm_i_data_rcv_st_handler,
                                                              sm_esc_found_st_handler,
                                                              sm_su_state_stop_st_handler,
                                                              sm_i_state_stop_st_handler};

void sm_processInput(struct receiver_state_machine *st_machine, uint8_t receivedByte)
{

if(st_machine->currentByte_idx == st_machine->allocatedMemory){

    st_machine->allocatedMemory *= 2;
    st_machine->frame = realloc(st_machine->frame, st_machine->allocatedMemory);
    log_debug(log_fp,"\n\n REALLOC MEMORY %ld\n\n", st_machine->allocatedMemory);
  }


   (*rcv_event_handlers[st_machine->currentState])(st_machine,receivedByte);

}
