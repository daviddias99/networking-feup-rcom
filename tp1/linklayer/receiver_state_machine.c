#include "receiver_state_machine.h"



void sm_start_st_handler(struct su_frame_rcv_state_machine *st_machine, uint8_t receivedByte)
{

  log_debug("STM: At start state");

  if (receivedByte == FLAG)
  {
    st_machine->currentState = R_STATE_FLAG_RCV;
    st_machine->frame[FLAG_START_INDEX] = receivedByte;
  }

  return;
}

void sm_flag_st_handler(struct su_frame_rcv_state_machine *st_machine, uint8_t receivedByte)
{

  log_debug("STM: At flag state");

  if (receivedByte == ADDR_TRANSM_COMMAND)
  {

    st_machine->currentState = R_STATE_A_RCV;
    st_machine->frame[ADDR_INDEX] = receivedByte;
  }
  else if (receivedByte == FLAG)
    return;
  else
    st_machine->currentState = R_STATE_START;

  return;
}

void sm_a_rcv_st_handler(struct su_frame_rcv_state_machine *st_machine, uint8_t receivedByte)
{

  log_debug("STM: At A state");

  if (receivedByte == CONTROL_SET)
  { // if the received command is to establish a communication (S or U)

    st_machine->currentState = R_STATE_SU_C_RCV;
    st_machine->frame[CTRL_INDEX] = receivedByte;
    st_machine->connectionEstablished = false;
  }
  else if (receivedByte == FLAG)
    st_machine->currentState = R_STATE_FLAG_RCV;
  else if (((receivedByte == 0x00) || (receivedByte == 0x40)) && (st_machine->connectionEstablished))
  { // if the received command is to receive data (I)
    st_machine->frame[CTRL_INDEX] = receivedByte;
    st_machine->currentState = R_STATE_I_C_RCV;
  }
  else
    st_machine->currentState = R_STATE_START;

  return;
}

void sm_su_c_rcv_st_handler(struct su_frame_rcv_state_machine *st_machine, uint8_t receivedByte)
{

  log_debug("STM: At SU C RCV state");
  if (receivedByte == (st_machine->frame[CTRL_INDEX] ^ st_machine->frame[ADDR_INDEX]))
  {
    st_machine->currentState = R_STATE_SU_BCC1_OK;
    st_machine->frame[BCC_INDEX] = receivedByte;
  }
  else if (receivedByte == FLAG)
    st_machine->currentState = R_STATE_FLAG_RCV;
  else
    st_machine->currentState = R_STATE_START;

  return;
}

void sm_i_c_rcv_st_handler(struct su_frame_rcv_state_machine *st_machine, uint8_t receivedByte)
{

  log_debug("STM: At I C RCV state");

  if (receivedByte == (st_machine->frame[CTRL_INDEX] ^ st_machine->frame[ADDR_INDEX]))
  { // check the first BCC
    log_debug("STM: Correct header bcc");
    st_machine->currentState = R_STATE_I_DATA_RCV;
    st_machine->frame[BCC_INDEX] = receivedByte;
    st_machine->currentByte_idx = 4;
  }
  else if (receivedByte == FLAG)
    st_machine->currentState = R_STATE_FLAG_RCV;
  else
    st_machine->currentState = R_STATE_START;

  return;
}

void sm_i_data_rcv_st_handler(struct su_frame_rcv_state_machine *st_machine, uint8_t receivedByte)
{

  log_debug("STM: At Data RCV state");

  if (receivedByte == 0x7d)
  { // escape character must be converted to original chararacter

    st_machine->currentState = R_STATE_ESCAPE_FOUND;
  }
  else if (receivedByte == FLAG)
  { // end of the frame. This means the previous byte was the BCC of the data and must be checked

    if(st_machine->currentByte_idx < 6){    // the end flag was received but no bcc and or data was received

      st_machine->currentState = R_STATE_START;
      return;
    }

    st_machine->frame[st_machine->currentByte_idx] = receivedByte;

    // BCC2 checking should not be done by the state machine

    // bool isDataBCCValid = valid_data_bcc(st_machine->frame, st_machine->currentByte_idx + 1); // calculate the data bcc

    // if (v)
    // {
    //   st_machine->currentState = R_STATE_I_STOP;
    // }
    // else
    // {
    //   st_machine->currentState = R_STATE_START;
    // }

    st_machine->currentState = R_STATE_I_STOP;
  }
  else
  {
    st_machine->frame[st_machine->currentByte_idx] = receivedByte; // normal data byte
    st_machine->currentByte_idx++;
  }

  return;
}

void sm_esc_found_st_handler(struct su_frame_rcv_state_machine *st_machine, uint8_t receivedByte)
{

  log_debug("STM: At ESCAPE Code Found state");

  if (receivedByte == 0x5d)
  {

    st_machine->frame[st_machine->currentByte_idx] = 0x7d; // parse escape byte
    st_machine->currentByte_idx++;
    st_machine->currentState = R_STATE_I_DATA_RCV;
  }
  else if (receivedByte == 0x5e)
  {
    st_machine->frame[st_machine->currentByte_idx] = 0x7e; // parse flag byte
    st_machine->currentByte_idx++;
    st_machine->currentState = R_STATE_I_DATA_RCV;
  }
  else
  {
    st_machine->currentState = R_STATE_START;
  }

  return;
}

void sm_su_bcc1_st_handler(struct su_frame_rcv_state_machine *st_machine, uint8_t receivedByte)
{

  log_debug("STM: At SU_bcc1 state");
  if (receivedByte == FLAG)
  {

    st_machine->currentState = R_STATE_SU_STOP;
    st_machine->frame[FLAG_END_INDEX] = receivedByte;
    st_machine->connectionEstablished = true;
  }
  else
    st_machine->currentState = R_STATE_START;

  return;
}

void sm_su_state_stop_st_handler(struct su_frame_rcv_state_machine *st_machine, uint8_t receivedByte)
{

  log_debug("STM: At SU stop state");
  if (receivedByte == FLAG)
  {

    st_machine->currentState = R_STATE_FLAG_RCV;
    st_machine->frame[FLAG_END_INDEX] = receivedByte;
  }
  else
    st_machine->currentState = R_STATE_START;

  return;
}

void sm_i_state_stop_st_handler(struct su_frame_rcv_state_machine *st_machine, uint8_t receivedByte)
{

  log_debug("STM: At I stop state");
  if (receivedByte == FLAG)
  {

    st_machine->currentState = R_STATE_FLAG_RCV;
    st_machine->frame[FLAG_END_INDEX] = receivedByte;
  }
  else
    st_machine->currentState = R_STATE_START;

  return;
}

void sm_processInput(struct su_frame_rcv_state_machine *st_machine, uint8_t receivedByte)
{

  switch (st_machine->currentState)
  {

  case R_STATE_START:

    sm_start_st_handler(st_machine, receivedByte);
    return;

  case R_STATE_FLAG_RCV:

    sm_flag_st_handler(st_machine, receivedByte);
    return;

  case R_STATE_A_RCV:

    sm_a_rcv_st_handler(st_machine, receivedByte);
    return;

  case R_STATE_I_C_RCV:

    sm_i_c_rcv_st_handler(st_machine, receivedByte);
    return;

  case R_STATE_I_DATA_RCV:

    sm_i_data_rcv_st_handler(st_machine, receivedByte);
    return;

  case R_STATE_ESCAPE_FOUND:

    sm_esc_found_st_handler(st_machine, receivedByte);
    return;

  case R_STATE_SU_C_RCV:

    sm_su_c_rcv_st_handler(st_machine, receivedByte);
    return;

  case R_STATE_SU_BCC1_OK:

    sm_su_bcc1_st_handler(st_machine, receivedByte);
    return;

  case R_STATE_SU_STOP:

    sm_su_state_stop_st_handler(st_machine,receivedByte);
    return;

  case R_STATE_I_STOP:
    sm_i_state_stop_st_handler(st_machine, receivedByte);
  }
}
