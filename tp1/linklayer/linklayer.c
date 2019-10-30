#include "linklayer.h"
#include "ll_consts.h"
#include "receiver_state_machine.h"
#include "transmitter_state_machine.h"

/**
 * @brief Opens the serial port for reading and writting and stores the old serial port configuration
 * 
 * @param port            integer identification of the serial port to be opened
 * @return int            file descriptor of the opened port (-1 if error)
 */
static int serial_port_setup(int port);

/**
 * @brief Opens a connection with a receiver by sending a SET message and waiting for a UA
 * 
 * @param fd              integer identification of the serial port to be opened
 * @return int            zero upon sucess, non-0 otherwise
 */
static int transmitter_open(int fd);

/**
 * @brief Opens a connection by waiting for a SET message from a transmitter
 * 
 * @param fd              integer identification of the serial port to be opened
 * @return int            zero upon sucess, non-0 otherwise
 */
static int receiver_open(int fd);

/**
 * @brief Closes an open connection by sending a DISC message, waiting for a DISC response from the receiver and sending a UA back
 * 
 * @param fd              integer identification of the serial port to be closed
 * @return int            zero upon sucess, non-0 otherwise
 */
static int transmitter_close(int fd);

/**
 * @brief Closes an open connection by waiting for a DISC message from the transmitter, sending  a DISC response and receiving a UA back
 * 
 * @param fd              integer identification of the serial port to be closed
 * @return int            zero upon sucess, non-0 otherwise
 */
static int receiver_close(int fd);

/**
 * @brief Writes a frame of a specific type to the file with fd received as argument 
 * 
 * @param fd              file descriptor of serial port to write
 * @param type            can be OPEN, CLOSE or DATA
 * @param buffer          frame to be sent in case of OPEN or CLOSE type, data to be sent in I frame in case of DATA type
 * @param length          irrelevant in case of OPEN or CLOSE type, length of the data to be sent in I frame in case of DATA type
 * @return int 
 */
static int write_frame(int fd, int type, uint8_t *buffer, size_t length);

/**
 * @brief Writes data as an I frame to file with fd received as argument (does necessary stuffing)
 * 
 * @param fd              file descriptor of serial port to write
 * @param buffer          data to be sent in the I frame
 * @param length          length of the buffer
 * @return int 
 */
static int write_data(int fd, uint8_t *buffer, int length);

/**
 * @brief Send a unnumbered acknowledgement to the connected partner
 * 
 * @param fd              file descriptor of the serial port where the UA should be sent to
 * @return int            zero upon success, non-zero otherwise
 */
static int sendUA(int fd);

/**
 * @brief Send a positive or negative acknowledgement to the connected partner
 * 
 * @param fd              file descriptor of the serial port where the UA should be sent to
 * @param type            true if ACK, false if NACK
 * @param sequence_no     sequence number to be associated with the acknowledgement (0 or 1)
 * @return int            zero upon success, non-zero otherwise
 */
static int sendAck(int fd,bool type, int sequence_no);


/**
 * @brief Checks if the data bcc of a type-I frame is correct
 * 
 * @param frame           type-I frame
 * @param frame_size      size of the frame
 * @return true           the data bcc of the frame is valid
 * @return false          the data bcc of the frame is not valid
 */
static bool valid_data_bcc(uint8_t frame[], size_t frame_size);

/**
 * @brief Build a S/U type frame with the given parameters
 * 
 * @param buf             buffer where the frame must be built
 * @param address         address field of the frame
 * @param control         control field of the frame
 */
static void build_su_frame(uint8_t *buf, int address, int control);

/**
 * @brief 
 * 
 * @param fd 
 * @param frame 
 * @param frame_size 
 * @param buffer 
 * @return int 
 */
static int process_read_i_frame(int fd, uint8_t frame[], size_t frame_size, uint8_t *buffer);
static int process_read_su_frame(int fd, uint8_t frame[]);
static void alarm_handler(int signo);

static void init_logging();

typedef struct linklayer
{

  char port[20];
  int baudRate;
  unsigned int sequenceNumber;
  int role;
  bool connectionEstablished;
  struct receiver_state_machine * rcv_st_machine;

} linklayer_info;

linklayer_info connection_info;

static struct termios oldtio; // Starting serial port settings (set after successful serial_port_setup)
int timedOut = 1, numTries = 1;
static FILE *log_fp = NULL;

static void alarm_handler(int signo)
{
  UNUSED(signo);
  printf("Alarm # %d\n", numTries);
  timedOut = 1;
  numTries++;
}

int sendUA(int fd)
{

  uint8_t response[SU_FRAME_SIZE];
  build_su_frame(response, ADDR_RECEIV_RES, CONTROL_UA);

  int res = write(fd, response, SU_FRAME_SIZE);
  log_debug(log_fp,"RECEIVER: UA sent to transmitter(%x %x %x %x %x) (%d bytes written)\n", response[0], response[1], response[2], response[3], response[4], res);

  return 0;
}

int sendAck(int fd, bool type, int sequence_no)
{
  // Build the control byte of the ACK frame
  uint8_t sequence_byte = sequence_no ? BIT(7) : 0x00;
  uint8_t control_byte = type ? (CONTROL_RR_BASE | sequence_byte) : (CONTROL_REJ_BASE | sequence_byte);
  uint8_t response[SU_FRAME_SIZE];

  build_su_frame(response, ADDR_RECEIV_RES, control_byte);
  int res = write(fd, response, SU_FRAME_SIZE);

  if (type)
    log_debug(log_fp,"RECEIVER: ACK(%d) sent to transmitter(%x %x %x %x %x) (%d bytes written)\n", sequence_no, response[0], response[1], response[2], response[3], response[4], res);
  else
    log_debug(log_fp,"RECEIVER: NACK(%d) sent to transmitter(%x %x %x %x %x) (%d bytes written)\n", sequence_no, response[0], response[1], response[2], response[3], response[4], res);

  return 0;
}

bool valid_data_bcc(uint8_t frame[], size_t frame_size)
{

  const int BCC2_INDEX = frame_size - 2;
  const int I_FRAME_DATA_END_INDEX = frame_size - 3;

  uint8_t bcc_value = frame[BCC2_INDEX];
  uint8_t calculated_value = 0;

  for (int i = I_FRAME_DATA_START_INDEX; i <= I_FRAME_DATA_END_INDEX; i++)
    calculated_value ^= frame[i];

  return bcc_value == calculated_value;
}

int process_read_su_frame(int fd, uint8_t frame[])
{

  if (frame[CTRL_INDEX] == CONTROL_SET)
  {

    sendUA(fd);                         // Send unnumbered acknowledgement to sender
    connection_info.sequenceNumber = 0; // The data transfer is about to start, so the sequence number is set to 0
    connection_info.rcv_st_machine->connectionEstablished = true;

    return CONTROL_SET;
  }
  else if (frame[CTRL_INDEX] == CONTROL_DISC)
  {
    return CONTROL_DISC;
  }
  else if (frame[CTRL_INDEX] == CONTROL_UA)
  {

    return CONTROL_UA;
  }

  return 0;
}

int process_read_i_frame(int fd, uint8_t frame[], size_t frame_size, uint8_t *buffer)
{

  const int I_FRAME_DATA_END_INDEX = frame_size - 3;
  bool isDataBCCValid = valid_data_bcc(frame, frame_size);

  if (isDataBCCValid)
  { // If the data bcc is valid check for the sequence number

    log_debug(log_fp,"RECEIVER: Data BCC is valid");
    unsigned int packetSeqNumber = frame[CTRL_INDEX] >> 6; // The sequence number is bit #6 of the control byte

    log_debug(log_fp,"RECEIVER: Received frame with seq=%d, desired was seq=%d", packetSeqNumber, connection_info.sequenceNumber);
    if (packetSeqNumber == connection_info.sequenceNumber)
    { // Correct sequence number, send ack with next seq number

      int nextSeqNumber = connection_info.sequenceNumber ? 0 : 1; // Calculate next sequence number
      sendAck(fd, true, nextSeqNumber);                           // Send ACK to receiver
      connection_info.sequenceNumber = nextSeqNumber;             // Update the sequence number

      // Copy read frame to frame storage
      int i;

      for (i = I_FRAME_DATA_START_INDEX; i <= I_FRAME_DATA_END_INDEX; i++)
        buffer[i - I_FRAME_DATA_START_INDEX] = frame[i];

      int read_data_byte_count = i - I_FRAME_DATA_START_INDEX;

      return read_data_byte_count;
    }
    else
    { // Duplicate frame, discard and send ack
      log_debug(log_fp,"RECEIVER: duplicate frame received");
      sendAck(fd, true, connection_info.sequenceNumber);
    }
  }
  else
  { // If the data bcc is invalid send nack asking for the same frame
    log_debug(log_fp,"RECEIVER: Data BCC is invalid");
    sendAck(fd, false, connection_info.sequenceNumber);
  }

  return -1;
}

void init_logging(){

  log_fp = fopen(LOG_FILE,"w");
  tsm_stm_set_log_fp(log_fp);
  rcv_stm_set_log_fp(log_fp);
}

int llopen(int port, int role)
{

  init_logging();

  // Setup timeout alarm
  struct sigaction alarm_action;
  memset(&alarm_action, 0, sizeof alarm_action);
  alarm_action.sa_handler = alarm_handler;
  sigaction(SIGALRM, &alarm_action, NULL);

  // Setup the serial port
  int fd = serial_port_setup(port);

  connection_info.role = role;

  switch (role)
  {
  case TRANSMITTER:
    return transmitter_open(fd);
  case RECEIVER:
    return receiver_open(fd);
  default:
    return -1;
  }
}

int serial_port_setup(int port)
{
  char port_path[PORT_PATH_LENGTH];
  sprintf(port_path, "/dev/ttyS%d", port);

  int fd = open(port_path, O_RDWR | O_NOCTTY);
  if (fd < 0)
  {
    perror(port_path);
    return -1;
  }

  struct termios newtio;

  if (tcgetattr(fd, &oldtio) == -1)
  { // Save current port settings
    perror("tcgetattr");
    return -1;
  }

  // Serial port configuration
  bzero(&newtio, sizeof(newtio));
  newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
  newtio.c_iflag = IGNPAR;
  newtio.c_oflag = 0;

  // Set input mode (non-canonical, no echo,...)
  newtio.c_lflag = 0;

  newtio.c_cc[VTIME] = VTIME_VALUE; // Inter-character timer unused
  newtio.c_cc[VMIN] = VMIN_VALUE;   // Blocking read until 5 chars received

  tcflush(fd, TCIOFLUSH);

  if (tcsetattr(fd, TCSANOW, &newtio) == -1)
  {
    perror("tcsetattr");
    return -1;
  }

  log_debug(log_fp,"Serial port configuration set (port_path:%s)", port_path);

  return fd;
}

int transmitter_open(int fd)
{
  log_debug(log_fp,"TRANSMITTER: Establishing connection...");

  // Frame building
  uint8_t frame[SU_FRAME_SIZE];
  build_su_frame(frame, ADDR_TRANSM_COMMAND, CONTROL_SET);

  // Linklayer connection info initial values
  connection_info.connectionEstablished = false;
  connection_info.sequenceNumber = 0;

  // Write SU-Open frame
  return write_frame(fd, OPEN, frame, 0);
}

int receiver_open(int fd)
{

  int read_bytes_cnt;

  // Receiver state machine setup
  connection_info.rcv_st_machine = create_rcv_state_machine();

  // Linklayer connection info initial values
  connection_info.sequenceNumber = 0;

  // Frame listening
  while (true)
  {
    uint8_t currentByte;

    // Read a byte from the serial port
    // Returns after a char has been read or after timer expired (see VMIN and TIME)
    read_bytes_cnt = read(fd, &currentByte, 1);
    log_debug(log_fp,"RECEIVER: received byte(0x%x - char:%c)(read %d bytes)", currentByte, (char)currentByte, read_bytes_cnt);

    // State-machine processes the read byte
    sm_processInput(connection_info.rcv_st_machine, currentByte);

    // If the state machine reached it's stopping state process the frame
    if (connection_info.rcv_st_machine->currentState == R_STATE_SU_STOP)
    {
      process_read_su_frame(fd, connection_info.rcv_st_machine->frame);
      return fd;
    }
  }

  return 0;
}

int write_data(int fd, uint8_t *buffer, int length)
{

  int data_bytes_written = 0;
  int total_bytes_written = 0;
  int nWritten = 0;

  // Build the data frame header
  uint8_t frame[I_FRAME_HEADER_SIZE];
  frame[FLAG_START_INDEX] = FLAG;
  frame[ADDR_INDEX] = ADDR_TRANSM_COMMAND;
  frame[CTRL_INDEX] = connection_info.sequenceNumber ? 0x40 : 0x00;
  frame[BCC1_INDEX] = frame[1] ^ frame[2];

  // Write header to serial port
  write(fd, frame, I_FRAME_HEADER_SIZE);

  total_bytes_written += I_FRAME_HEADER_SIZE;

  uint8_t bcc2 = 0x00;

  // Data writting to serial port (byte by byte)
  for (int i = 0; i < length; i++)
  {

    // Update BCC2
    bcc2 ^= buffer[i];

    if (buffer[i] == FLAG || buffer[i] == ESC)
    {

      // Handling byte stuffing
      frame[0] = ESC;
      frame[1] = buffer[i] ^ ESC_XOR;
      nWritten = write(fd, frame, 2);

      data_bytes_written += (nWritten == 2) ? 1 : nWritten;
      total_bytes_written += nWritten;
    }
    else
    {
      nWritten = write(fd, &buffer[i], 1);
      data_bytes_written += nWritten;
      total_bytes_written += nWritten;
    }
  }

  // Write the Data bcc

  if (bcc2 == FLAG || bcc2 == ESC)
  { // Stuff the data bcc if necessary
    frame[0] = ESC;
    frame[1] = bcc2 ^ ESC_XOR;
    nWritten = write(fd, frame, 2);
    total_bytes_written += nWritten;
  }
  else
  {
    nWritten = write(fd, &bcc2, 1);
    total_bytes_written += nWritten;
  }

  // Write the final flag
  frame[0] = FLAG;
  nWritten = write(fd, frame, 1);
  total_bytes_written += nWritten;

  log_debug(log_fp,"TRANSMITTER: Data frame sent to Receiver(%d bytes written - %d data bytes written) - header: 0x%x 0x%x 0x%x 0x%x \n", total_bytes_written, data_bytes_written, frame[0], frame[1], frame[2], frame[3]);

  return data_bytes_written;
}

int llwrite(int fd, uint8_t *buffer, int length)
{

    if(!connection_info.connectionEstablished)
      return -1;

  return write_frame(fd, DATA, buffer, length);
}

int llread(int fd, uint8_t *buffer)
{

  int res;

  if(!connection_info.connectionEstablished)
    return -1;

  // Initialize the received frame processing state-machine
  reset_rcv_state_machine(connection_info.rcv_st_machine);

  log_debug(log_fp,"RECEIVER: started listening for data");

  // Listen for data
  while (true)
  {

    uint8_t currentByte;

    // Read a byte from the serial port
    // Returns after a char has been read or after timer expired (see VMIN and TIME)
    res = read(fd, &currentByte, 1);

    log_debug(log_fp,"RECEIVER: received byte(0x%x - char:%c)(read %d bytes)", currentByte, (char)currentByte, res);

    // Process the read byte
    sm_processInput(connection_info.rcv_st_machine, currentByte);

    if (connection_info.rcv_st_machine->currentState == R_STATE_SU_STOP)
    { // SU frame reading stop-state reached

      if (process_read_su_frame(fd, connection_info.rcv_st_machine->frame) == CONTROL_DISC)
        return 0;
    }
    else if (connection_info.rcv_st_machine->currentState == R_STATE_I_STOP)
    { // Data frame reading stop-state reached

      int nRead = process_read_i_frame(fd, connection_info.rcv_st_machine->frame, connection_info.rcv_st_machine->currentByte_idx, buffer);

      if (nRead >= 0)
        return nRead;
    }
  }

  return -1;
}

void build_su_frame(uint8_t *buf, int address, int control)
{
  buf[FLAG_START_INDEX] = FLAG;
  buf[ADDR_INDEX] = address;
  buf[CTRL_INDEX] = control;
  buf[BCC1_INDEX] = address ^ control;
  buf[FLAG_END_INDEX] = FLAG;
}

int llclose(int fd)
{
  log_debug(log_fp,"\n\n-CLOSING CONNECTION...\n");

  switch (connection_info.role)
  {
  case TRANSMITTER:
    transmitter_close(fd);
    break;
  case RECEIVER:
    receiver_close(fd);
    break;
  default:
    break;
  }

  sleep(1);

  // Rollback serial port configuration
  if (tcsetattr(fd, TCSANOW, &oldtio) == -1)
  {
    perror("tcsetattr");
    exit(-1);
  }

  return close(fd);
}

int transmitter_close(int fd)
{

  // Frame building
  uint8_t frame[SU_FRAME_SIZE];
  build_su_frame(frame, ADDR_TRANSM_COMMAND, CONTROL_DISC);

  // Send a DISC command to the receiver using a SU frame
  // The receiver should respond with a DISC and the transmitter will also send a unnumbered
  return write_frame(fd, CLOSE, frame, 0);
}

int write_frame(int fd, int type, uint8_t *buffer, size_t length)
{
  int n_written, res;
  struct transmitter_state_machine st_machine;

  numTries = 1;
  timedOut = 1;

  // Calculate the next sequence number (if 0 -> 1, if 1 -> 0)
  int nextSequenceNumber = (connection_info.sequenceNumber + 1) % 2;

  // Try to send the frame (if the sending failed 3 times, return with error)
  while (numTries <= MAX_TIMEOUT_RETRY_CNT)
  {
    // Reset state-machine
    st_machine.currentState = T_STATE_START;

    // Send data or S/U frame
    if (type == DATA)
    {
      n_written = write_data(fd, buffer, length);
    }
    else
    {
      n_written = write(fd, buffer, SU_FRAME_SIZE);
      log_debug(log_fp,"TRANSMITTER: SU frame sent to transmitter(%x %x %x %x %x) (%d bytes written)\n", buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], res);
    }

    // Activate 3 second alarm, for timeout
    alarm(3);
    timedOut = 0;

    // Wait for answer from the receiver (until timeout happens)
    while (!timedOut)
    {

      uint8_t currentByte;

      // Read a byte from the serial port
      // Returns after a char has been read or after timer expired (see VMIN and TIME)
      res = read(fd, &currentByte, 1);

      log_debug(log_fp,"TRANSMITTER: received byte(0x%x - char:%c)(read %d bytes)", currentByte, (char)currentByte, res);

      // State-machine processes the read byte
      tsm_process_input(&st_machine, currentByte);

      if (st_machine.currentState == T_STATE_STOP)
      { // SU frame reading stop-state reached

        if (type == DATA)
        {

          if (st_machine.frame[2] == ((nextSequenceNumber << 7) | CONTROL_RR_BASE))
          {
            // Expected ACK received (with the next sequence number)
            // Turn off alarm and return real number of data bytes written
            // Update the sequence number (if 1 -> 0, if 0 -> 1)

            alarm(0);
            log_debug(log_fp,"TRANSMITTER: ACK(%d) received", nextSequenceNumber);
            connection_info.sequenceNumber = nextSequenceNumber;
            return n_written;
          }
          else if (st_machine.frame[2] == ((connection_info.sequenceNumber << 7) | CONTROL_REJ_BASE))
          {
            // NACK received, try to send the same data frame again
            // numTries is reseted because no timeout occured (a transmission error happened)

            log_debug(log_fp,"TRANSMITTER: NACK(%d) received", connection_info.sequenceNumber);
            numTries = 1;
            break;
          }
        }
        else if (type == OPEN)
        {
          // Turn off alarm
          alarm(0);
          return fd;
        }
        else if (type == CLOSE)
        {
          // Turn off alarm
          alarm(0);

          // Receiver responds to the disconnect call. Send UA and return the serial port file descriptor
          uint8_t response[SU_FRAME_SIZE];
          build_su_frame(response, ADDR_TRANSM_RES, CONTROL_UA);

          int res = write(fd, response, SU_FRAME_SIZE);
          log_debug(log_fp,"TRANSMITTER: UA sent to transmitter(%x %x %x %x %x) (%d bytes written)\n", response[0], response[1], response[2], response[3], response[4], res);

          return fd;
        }
      }
    }
  }

  // If the max number of retries is exceed return -1 (error)
  return -1;
}

int receiver_close(int fd)
{

  int res;
  bool disconnecting = false; // True of a disconnecting call from the transmitter was received

  // Setup the state-machine
  reset_rcv_state_machine(connection_info.rcv_st_machine);

  // Frame building
  uint8_t frame[SU_FRAME_SIZE];
  build_su_frame(frame, ADDR_RECEIV_COMMAND, CONTROL_DISC);

  //Alarm setup
  timedOut = 0;
  numTries = 1;

  log_debug(log_fp,"RECEIVER: started listening for disconnecting calls");

  // Wait for disconnecting call from the transmitter
  while (numTries <= MAX_TIMEOUT_RETRY_CNT)
  {

    uint8_t currentByte;

    // Read a byte from the serial port
    // Returns after a char has been read or after timer expired (see VMIN and TIME)
    res = read(fd, &currentByte, 1);

    log_debug(log_fp,"RECEIVER: received byte(0x%x - char:%c)(read %d bytes)", currentByte, (char)currentByte, res);

    // State-machine processes the read byte
    sm_processInput(connection_info.rcv_st_machine, currentByte); /* state-machine processes the read byte */

    // If the receiver received a disconnecting call and sent the responde but the time for receiving the
    // acknowledgement of that disconnect expired, send the response again
    if (disconnecting && timedOut)
    {
      log_debug(log_fp,"RECEIVER: disc timeout happened, trying again (tried %d time(s))", numTries);
      res = write(fd, frame, SU_FRAME_SIZE);
      alarm(3);
      timedOut = 0;
    }

    if (connection_info.rcv_st_machine->currentState == R_STATE_SU_STOP)
    { // Complete frame received from transmitter

      int process_result = process_read_su_frame(fd, connection_info.rcv_st_machine->frame);

      if (process_result == CONTROL_DISC)
      { // Frame was a DISC command, send a DISC command back and wait for a UA
        log_debug(log_fp,"RECEIVER: DISC received, sending DISC to trasmitter");
        res = write(fd, frame, SU_FRAME_SIZE);

        // Turn previous alarm off and setup a new one
        alarm(0);
        alarm(3);
        timedOut = 0;
        disconnecting = true;
      }
      else if ((process_result == CONTROL_UA) && disconnecting)
      { // Frame was a UA and the disconnecting was started previously (by receing a DISC command).
        // Disconnect sucessful
        log_debug(log_fp,"RECEIVER: UA received, returning...");
        alarm(0);
        destroy_rcv_state_machine(connection_info.rcv_st_machine);
        return 0;
      }
    }
  }

  return -1;
}
