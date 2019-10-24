#include "tlv.h"

int main(void) {

    char* file_name = "this is the file name";

    tlv* tlv_ptr_int = create_tlv_int(FILE_SIZE, 10000);
    tlv* tlv_ptr_str = create_tlv_str(FILE_NAME, file_name);

    printf("type : %d \n", tlv_ptr_int->type);
    printf("length : %d \n", tlv_ptr_int->length);
    printf("value : %d %d %d %d \n", tlv_ptr_int->value[0], tlv_ptr_int->value[1], tlv_ptr_int->value[2], tlv_ptr_int->value[3]);

    printf("\n");

    print_tlv(tlv_ptr_int);    

    printf("\n");

    printf("type : %d \n", tlv_ptr_str->type);
    printf("length : %d \n", tlv_ptr_str->length);
    printf("value : %s \n", tlv_ptr_str->value);

    return 0;
}