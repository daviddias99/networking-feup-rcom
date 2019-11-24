#include "./app_receive.h"

static FILE* log_fp = NULL;

void app_rcv_set_log_fp(FILE* fp){
  log_fp = fp;
}

int receive_file(int port) {

    // open the serial port
    log_debug(log_fp, "APP_R: Establishing communication");
    int port_fd = llopen(port, RECEIVER);
    printf("port fd : %d\n", port_fd);
    if (port_fd < 0) {
        printf("No communication established\n");
        exit(1);
    }

    log_debug(log_fp, "APP_R: Communitcation established");

    uint8_t packet[2000];
    int bytes_read = llread(port_fd, packet);

    if (packet[0] != START) {
        printf("Expected start packet\n");
        exit(1);
    }

    log_debug(log_fp, "APP_R: START packet read");

    control_info* start_info = create_control_info(packet, bytes_read);

    log_debug(log_fp, "APP_R: Receiving file(%s, %d)...", start_info->file_name, start_info->file_size);

    FILE* file_ptr = fopen(start_info->file_name, "wb");

    size_t progress = 0;
    while ((bytes_read = llread(port_fd, packet)) > 0) {


        if (packet[0] == END) {
            log_debug(log_fp, "APP_R: END packet read");
            break;
        }
        else if (packet[0] != DATA) {
            printf("Found unexpected type : %d\n\n", packet[0]);
            exit(1);
        }

        log_debug(log_fp, "APP_R: DATA packet read (%d bytes read)", bytes_read);
        fwrite(packet + 4, 1, bytes_read - 4, file_ptr);
        log_debug(log_fp, "APP_R: %d bytes writen to file(%s)", bytes_read - 4, start_info->file_name);

        progress += bytes_read - 4;
        progress_bar("Receiving file", progress, start_info->file_size);
    }

    if (bytes_read < 0) {
        printf("Error while reading\n");
        exit(1);
    }

    control_info* end_info = create_control_info(packet, bytes_read);

    // Check if start and end packets carried the same information
    if (compare_control_info(start_info, end_info) == 0) {
        printf("Start and end packets carried different information\n");
    }

    destroy_control_info(start_info);
    destroy_control_info(end_info);

    fclose(file_ptr);
    llclose(port_fd);

    return 0;
}

control_info* create_control_info(uint8_t* packet, const size_t packet_size) {

    if (packet[0] != END && packet[0] != START) {
        printf("Unexpected packet type\n");
        return NULL;
    }

    control_info* info = malloc(sizeof(control_info));
    if (info == NULL) {
        printf("Unable to allocate memory\n");
        return NULL;
    }

    size_t packet_index = 1;

    while (packet_index < packet_size) {
        int type = packet[packet_index];
        packet_index++;
        uint8_t length = packet[packet_index];
        packet_index++;

        switch (type) {
            case FILE_SIZE:
            {
                info->file_size = array_to_int(packet + packet_index);
                break;
            }
            case FILE_NAME:
            {
                info->file_name = strndup((char*)packet + packet_index, length);
                printf("file name : %s\n", info->file_name);
                if (info->file_name == NULL) {
                    printf("Error allocating memory for file name\n");
                    return NULL;
                }
                break;
            }
            default:
            {
                printf("Found unexpected parameter type\n");
                return NULL;
            }
        }

        packet_index += length;
    }
    return info;
}

void destroy_control_info(control_info* info) {
    free(info);
}

int compare_control_info(control_info* start_info, control_info* end_info) {
    if (start_info->file_size != end_info->file_size)
        return 0;
    if (strcmp(start_info->file_name, end_info->file_name) != 0)
        return 0;

    return 1;
}
