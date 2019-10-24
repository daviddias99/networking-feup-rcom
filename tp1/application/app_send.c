
#include "./app_send.h"
#include "./aux.h"
#include "./tlv.h"

int send_file(char* file_path) {

    // file contents
    uint8_t file_data[MAX_FILE_SIZE_B];
    int file_fd;

    file_fd = open(file_path, O_RDONLY);

    if (file_fd == -1){
        perror("file does not exist");
        exit(1);
    }

    int read_status = read(file_fd, file_data, MB_TO_B(MAX_FILE_SIZE_MB));

    if (read_status == -1){
        printf("file reading error");
        exit(2);
    }

    file_info file_info;
    file_info.path = strdup(file_path);
    file_info.size = strlen(file_data);

    tlv* tlv_list[2];
    tlv_list[0] = create_tlv_int(FILE_SIZE, file_info.size);
    tlv_list[1] = create_tlv_str(FILE_NAME, file_info.path);

    uint8_t* packet;
    if (build_start_packet(packet, tlv_list) < 0) {
        printf("Unable to build start packet\n");
        return -1;
    }

    for (uint8_t i = 0; i < sizeof(tlv_list) / sizeof(tlv*); i++) {
        destroy_tlv(tlv_list[i]);
    }
    free(packet);

    return 0;
}

int build_start_packet(uint8_t* packet, tlv* tlv_list[]) {
    
    size_t packet_size = 1;
    for (uint8_t i = 0; i < sizeof(tlv_list) / sizeof(tlv*); i++)
        packet_size += tlv_list[i]->length;

    packet = malloc(packet_size);
    if (packet == NULL) {
        printf("Unable to allocate memory\n");
        return -1;
    }

    packet[0] = START;
    uint8_t packet_index = 1;
    for (uint8_t i = 0; i < sizeof(tlv_list) / sizeof(tlv*); i++) {
        
        packet[packet_index] = tlv_list[i]->type;
        packet_index++;

        packet[packet_index] = tlv_list[i]->length;
        packet_index++;

        for (uint8_t j = 0; j < tlv_list[i]->length; j++) {
            packet[packet_index] = tlv_list[i]->value[j];
            packet_index++;
        }
    }

    return 0;
}