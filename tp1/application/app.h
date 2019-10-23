#ifndef APP_H
#define APP_H

#define MAX_FILE_SIZE_MB            1
#define MB_TO_B(x)                  ((size_t) (x) << 20)   
#define MAX_FILE_SIZE_B             MB_TO_B(MAX_FILE_SIZE_MB)

#define MAX_PACKET_SIZE_B           4



typedef struct tl_package {

    uint8_t type;
    uint8_t length;
} tl_package;

enum control_type {
    DATA = 1,
    START,
    END
};

enum data_type {
    FILE_SIZE = 0,
    FILE_NAME
};
        


#endif