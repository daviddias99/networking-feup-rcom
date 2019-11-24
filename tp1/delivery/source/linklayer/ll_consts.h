
// General Settings
#define BAUDRATE                    B9600   // baudrate used by the serial port            
#define MAX_TIMEOUT_RETRY_CNT       3       // Number of timeouts for sending data to the partner that cause the ll functions to return uncessfully
#define VTIME_VALUE                 20      // VMIN value for the configuration of the serial port (ms before the timeout) 
#define VMIN_VALUE                  1       // VMIN value for the configuration of the serial port (min number of bytes read before read return)
#define PORT_PATH_LENGTH            11      // Max length of the port path
#define LOG_FILE                    "ll_log.txt"

// Frame header indices (S/U/I) - Note: S/U frames don't have data
#define FLAG_START_INDEX            0   // Starting delimiter flag byte
#define ADDR_INDEX                  1   // Address byte
#define CTRL_INDEX                  2   // Control byte
#define BCC1_INDEX                  3   // Error control byte
#define FLAG_END_INDEX              4   // Only in S/U frames

#define SU_FRAME_SIZE               5   // Number of bytes in a SU Frame
#define I_FRAME_HEADER_SIZE         4   // Number of bytes in a I Frame header
#define I_FRAME_DATA_START_INDEX    4   // Index at which the data bytes start appearing on a I frame

// Frame header byte values
#define FLAG                        0x7e  // delimiter flag used in transmission (signals start and end of a frame)
#define ADDR_TRANSM_COMMAND         0x03  // address for command sent from transmitter
#define ADDR_TRANSM_RES             0x01  // address for response sent from transmitter
#define ADDR_RECEIV_COMMAND         0x01  // address for command sent from receiver
#define ADDR_RECEIV_RES             0x03  // address for response sent from receiver
#define CONTROL_SET                 0x03  // set
#define CONTROL_DISC                0x0B  // disconnect
#define CONTROL_UA                  0x07  // unnumbered acknowledgment
#define CONTROL_RR_BASE             0x05  // receiver ready / positive acknowledgment
#define CONTROL_REJ_BASE            0x01  // reject / negative acknowledgment

#define ESC	                        0x7d  // escape char  
#define ESC_XOR                     0x20  // escape char xor

#define BIT(n)                      (0x01<<(n))

#define I_FRAME_START_SIZE  256          // starting size of the the I-type frame


#define DATA                        0       // Data frame type ID
#define CLOSE                       1       // Close frame type ID
#define OPEN                        2       // Open frame type ID

#define UNUSED(expr) (void)(expr);