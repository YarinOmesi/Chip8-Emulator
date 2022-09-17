#include "debug.h"
#include <stdio.h>
#define PRINT_BYTE(byte) printf("%s%s", bit_rep[byte >> 4], bit_rep[byte & 0x0F]);

const char *bit_rep[16] = {
        [ 0] = "____", [ 1] = "___1", [ 2] = "__1_", [ 3] = "__11",
        [ 4] = "_1__", [ 5] = "_1_1", [ 6] = "_11_", [ 7] = "_111",
        [ 8] = "1___", [ 9] = "1__1", [10] = "1_1_", [11] = "1_11",
        [12] = "11__", [13] = "11_1", [14] = "111_", [15] = "1111",
};

void display_dump(VM * vm){
    for (int i = 0; i < DISPLAY_BUFFER_SIZE; ++i) {
        PRINT_BYTE(vm->display->buffer[i]);
        if(((i+1) % 8 ) == 0 ){
            printf("\n");
        }
    }
    printf("\n");
}
