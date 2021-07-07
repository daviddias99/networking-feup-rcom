#include "app_util.h"

void log_control_packet(uint8_t* packet, uint8_t packet_size) {

    printf("-- CONTROL PACKET -- \n");
    printf("type: %d\n", packet[0]);

    uint8_t i = 1;
    uint8_t parameter_number = 0;
    while (i < packet_size) {
        printf("parameter number : %d\n", parameter_number);
        printf("    type : %d\n", packet[i]);
        i++;
        uint8_t length = packet[i];
        printf("    length : %d\n", length);
        i++;
        printf("    value :");
        for (uint8_t j = 0; j < length; j++) {
            printf(" %x", packet[i + j]);
        }
        printf("\n");
        i += length;
    }
}

void log_data_packet(uint8_t* packet) {
    printf("-- DATA PACKET -- \n");
    printf("control : %d\n", packet[0]);
    printf("sequence number : %d\n", packet[1]);

    size_t length = 256 * packet[2] + packet[3];
    printf("size : 256 * %d + %d = %ld\n", packet[2], packet[3], length);

    printf("data :");
    for (size_t i = 0; i < length; i++)
        printf(" %x", packet[4 + i]);
    printf("\n");
}


void progress_bar(const char* prefix, size_t count, size_t max_count) {

    int progress = count * 100 / max_count;
    
    fflush(stdout);
    printf("\r%s : %3d%% [", prefix, progress);

    for (uint8_t i = 0; i < progress; i++)
        printf("#");
    for (uint8_t i = progress; i < 100; i++)
        printf(" ");
    printf("]"); 

	if (progress == 100)
		printf("\n");
}
