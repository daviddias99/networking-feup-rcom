#include "./tlv.h"

tlv* create_tlv_int(data_type type, int value) {
    tlv* tlv_ptr;

    tlv_ptr = malloc(sizeof(tlv));
    if (tlv_ptr == NULL) {
        printf("Unable to allocate memory\n");
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
    
    int_to_array(value, tlv_ptr->value);

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
            int integer = array_to_int(tlv_ptr->value);          
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


void int_to_array(int integer, uint8_t* array) {
    array[0] = (integer >> 24) & 0xFF;
    array[1] = (integer >> 16) & 0xFF;
    array[2] = (integer >> 8) & 0xFF;
    array[3] = integer & 0xFF;
}

int array_to_int(uint8_t* array) {
    int integer = array[0] << 24;
    integer |= array[1] << 16;
    integer |= array[2] << 8;
    integer |= array[3];

    return integer;
}