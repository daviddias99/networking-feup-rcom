#ifndef APP_CONSTS_H
#define APP_CONSTS_H

#define MB_TO_B(x)                  ((size_t) (x) << 20)   
#define MAX_FILE_SIZE_MB            1
#define MAX_FILE_SIZE_B             MB_TO_B(MAX_FILE_SIZE_MB)
#define MAX_PACKET_SIZE_B           4


// Control Packet Type Macros
#define CTRLP_FILE_SIZE             0
#define CTRLP_FILE_NAME             1
        


#endif