#include "./tlv.h"

tlv* create_tlv_int(data_type type, int value) {
    tlv* tlv_ptr;

    tlv_ptr = malloc(sizeof(tlv));

    if (tlv_ptr == NULL) {
        perror("Unable to allocate memory");
        return NULL;
    }

    tlv_ptr->type = type;
    tlv_ptr->length = sizeof(int);
    
    tlv_ptr->value = malloc(sizeof(int));
    if (tlv_ptr->value == NULL) {
        free(tlv_ptr);
        perror("Unable to allocate memory");
        return NULL;
    }

    tlv_ptr->value_type = INT;
    
    tlv_ptr->value[0] = (value >> 24) & 0xFF;
    tlv_ptr->value[1] = (value >> 16) & 0xFF;
    tlv_ptr->value[2] = (value >> 8) & 0xFF;
    tlv_ptr->value[3] = value & 0xFF;

    return tlv_ptr;
}

tlv* create_tlv_str(data_type type, char* value) {
    tlv* tlv_ptr;

    tlv_ptr = malloc(sizeof(tlv));

    if (tlv_ptr == NULL) {
        printf("Unable to allocate memory");
        return NULL;
    }

    tlv_ptr->type = type;
    tlv_ptr->length = strlen(value);

    tlv_ptr->value_type = STRING;
    
    tlv_ptr->value = (uint8_t*) strdup(value);
    if (tlv_ptr->value == NULL) {
        free(tlv_ptr);
        printf("Unable to allocate memory");
        return NULL;
    }

    return tlv_ptr;
}

void destroy_tlv(tlv* tlv_ptr) {
    free(tlv_ptr->value);
    free(tlv_ptr);
}

void print_tlv(tlv* tlv_ptr) {
    printf("type : %d \n", tlv_ptr->type);
    printf("length : %d \n", tlv_ptr->length);
    printf("value : ");

    switch (tlv_ptr->value_type) {
        case STRING:
        {
            printf("%s \n", tlv_ptr->value);
            break;
        }
        case INT:
        {
            int integer = tlv_ptr->value[0] << 24;
            integer |= tlv_ptr->value[1] << 16;
            integer |= tlv_ptr->value[2] << 8;
            integer |= tlv_ptr->value[3];            
            printf("%d \n", integer);
            break;
        }
        default:
        {
            printf("\nCannot print value\n");
            break;
        }
    }
}