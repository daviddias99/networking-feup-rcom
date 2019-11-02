#include "./app_send.h"

static FILE* log_fp = NULL;

void app_snd_set_log_fp(FILE* fp){
  log_fp = fp;
}

int send_file(int port, char* file_path, size_t packet_size) {

    log_debug(log_fp,"APP_T: Start file(%s) sending to receiver procedure", file_path);

    log_debug(log_fp,"APP_T: Opening the file...");

    // open the file that will be sent
    int file_fd = open(file_path, O_RDONLY);
    if (file_fd == -1){
        perror("File does not exist \n");
        exit(1);
    }

    log_debug(log_fp,"APP_T: Retreiving file information...");

    // retreive file information, only used to get the size in this case
    struct stat file_st;
    if (fstat(file_fd, &file_st) < 0) {
        perror("Unable to get file info \n");
        exit(1);
    }

    // create control packet segments
    const uint8_t tlv_list_size = 2;
    tlv* tlv_list[tlv_list_size];
    tlv_list[0] = create_tlv_int(FILE_SIZE, file_st.st_size);
    tlv_list[1] = create_tlv_str(FILE_NAME, name_from_path(file_path));

    log_debug(log_fp, "APP_T: Creating control packet tlv segments...");
    log_debug(log_fp, "APP_T: TLV[0] -type:%d -length:%d -value:%d ", tlv_list[0]->type, tlv_list[0]->length, *(int*) tlv_list[0]->value);
    log_debug(log_fp, "APP_T: TLV[1] -type:%d -length:%d -value:%s ", tlv_list[0]->type, tlv_list[0]->length, (char*) tlv_list[1]->value);

    // build the control packet
    uint8_t* control_packet;
    int control_packet_size;
    if ((control_packet = build_control_packet(START, &control_packet_size, tlv_list, tlv_list_size)) == NULL) {
        printf("Unable to build start packet\n");
        exit(1);
    }

    log_debug(log_fp,"APP_T: Building control packet(%d bytes)", control_packet_size);

    uint8_t* file_data = malloc(sizeof(uint8_t) * packet_size);
    int bytes_read;

    // open the serial port
    int serial_port_fd;
    log_debug(log_fp,"APP_T: attempting to open serial port...");
    serial_port_fd = llopen(port, TRANSMITTER);

    while (serial_port_fd < 0) {
        sleep(1);
        log_debug(log_fp,"APP_T: attempting to open serial port...");
        serial_port_fd = llopen(port, TRANSMITTER);  
    }

    int nWritten;

    // send the start control packet

    do {
        printf("\nCONTROL PACKET SIZE SEND: %d\n\n", control_packet_size);
        nWritten = llwrite(serial_port_fd, control_packet, control_packet_size);
        log_debug(log_fp,"APP_T: Writting control packet(START) to serial port (%d bytes written)",nWritten);
        
        if (nWritten == -1)
            continue;
    } while (nWritten != control_packet_size);


    size_t progress = 0;
    // read chunks of the outbound file and send them to the receiver
    while ((bytes_read = read(file_fd, file_data, packet_size)) > 0) {
        uint8_t* data_packet;
        if ((data_packet = build_data_packet((uint8_t *) file_data, bytes_read)) == NULL) {
            perror("Error!\n");
            break;
        }

        // send the packet through the serial port
        do{
            nWritten = llwrite(serial_port_fd, data_packet, bytes_read + 4);
            log_debug(log_fp,"APP_T: Writting data packet to serial port (%d bytes written)", nWritten);

        } while (nWritten != bytes_read + 4);
        free(data_packet);

        progress += bytes_read;
        progress_bar("Sending file", progress, file_st.st_size);
    }

    if (bytes_read == -1){
        perror("Error while reading the file \n");
        exit(1);
    }

    // modify start packet to send end packet
    control_packet[0] = END;

    // send the END control packet
    do {
        nWritten = llwrite(serial_port_fd,control_packet,control_packet_size);
        log_debug(log_fp,"APP_T: Writting control packet(END) to serial port (%d bytes written)",nWritten);

    } while (nWritten != control_packet_size);


    for (size_t i = 0; i < sizeof(tlv_list) / sizeof(tlv*); i++) {
        destroy_tlv(tlv_list[i]);
    }

    // close the connection
    log_debug(log_fp,"APP_T: Closing the connection with the receiver..." );
    llclose(serial_port_fd);
    free(control_packet);
    free(file_data);
    close(file_fd);

    return 0;
}

uint8_t* build_control_packet(packet_type type, int* packet_size, tlv* tlv_list[], const uint8_t tlv_list_size) {

    *packet_size = 1;
    printf("control packet size : %d\n", *packet_size);

    for (uint8_t i = 0; i < tlv_list_size; i++) {
        *packet_size += tlv_list[i]->length;
        printf("control packet size : %d\n", *packet_size);
    }

    uint8_t* packet = malloc(*packet_size);
    if (packet == NULL) {
        printf("Unable to allocate memory\n");
        return NULL;
    }

    packet[0] = type;

    uint8_t packet_index = 1;
    for (uint8_t i = 0; i < tlv_list_size; i++) {

        packet[packet_index] = tlv_list[i]->type;
        packet_index++;

        packet[packet_index] = tlv_list[i]->length;
        packet_index++;

        *packet_size += 2;

        for (uint8_t j = 0; j < tlv_list[i]->length; j++) {
            packet[packet_index] = tlv_list[i]->value[j];
            packet_index++;
        }
    }

    return packet;
}


uint8_t* build_data_packet(uint8_t* data, size_t data_size) {
    static uint8_t sequence_number = 0;
    sequence_number %= 256;

    uint8_t* packet = malloc(4 + data_size);
    if (packet == NULL) {
        printf("Unable to allocate memory for data packet number : %d\n", sequence_number);
        return NULL;
    }

    packet[0] = DATA;
    packet[1] = sequence_number;
    packet[2] = data_size / 256;
    packet[3] = data_size % 256;

    uint8_t* ptr = memcpy(packet + 4, (uint8_t *) data, data_size);
    if (ptr != packet + 4) {
        printf("Failed to build data packet number : %d\n", sequence_number);
        free(packet);
        return NULL;
    }

    sequence_number++;

    return packet;
}

char* name_from_path(char* path) {

    char* name = path;
    for (size_t i = 0; i < strlen(path); i++) {
        if (path[i] == '/')
            name = path + i + 1;
    }

    return name;
}
