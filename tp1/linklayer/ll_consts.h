#define BAUDRATE B9600

// Frame of type S or U array indeces
#define FLAG_START_INDEX           0
#define ADDR_INDEX                 1
#define CTRL_INDEX                 2
#define BCC_INDEX                  3
#define FLAG_END_INDEX             4

#define SU_FRAME_SIZE              5
#define I_FRAME_HEADER_SIZE       4
#define I_FRAME_SIZE               255
#define PORT_PATH_LENGTH           11

// Frame of type S or U Header Slot Values
#define FLAG                  0x7e  //
#define ADDR_TRANSM_COMMAND   0x03  // 
#define ADDR_TRANSM_RES       0x01  //
#define ADDR_RECEIV_COMMAND   0x01  //
#define ADDR_RECEIV_RES       0x03  //
#define CONTROL_SET           0x03  // set
#define CONTROL_DISC          0x0B  // disconnect
#define CONTROL_UA            0x07  // unnumbered acknowledgment
#define CONTROL_RR_BASE       0x05  // receiver ready / positive acknowledgment
#define CONTROL_REJ_BASE      0x01  // reject / negative acknowledgment

#define ESC	0x7d
#define ESC_XOR 0x20

#define BIT(n)                (0x01<<(n))

#define VTIME_VALUE          20
#define VMIN_VALUE           1

#define I_FRAME_DATA_START_INDEX    4

#define DATA 0
#define CLOSE 1
#define OPEN 2
#define LOG_FILE            "ll_log.txt"