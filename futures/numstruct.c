#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

//is it better to stash a char with the number of bytes in the first value
//of the pointer, or just hold it in a struct?
//faster to get count in the struct - packing is a problem though
//slower to dereference - will use less space because byte boundaries?
//
//should do speed comparison between the struct and the char[]
//(can typedef the char into our own type for clarity)

/* REPLACE MASK AND '8' WITH UCHAR_MAX AND CHAR_BIT, RESPECTIVELY */

static unsigned char mask = UCHAR_MAX;

typedef struct NcNumber {
    unsigned char num;
    unsigned char *bytes;//array
} NcNumber;

NcNumber *newNumber(unsigned long long value){
    NcNumber *result = malloc(sizeof(NcNumber));
    int i;
    for (i = sizeof(unsigned long long); i > 0; --i){
        if (((value >> (8 *(i-1))) & 0xff) | mask != mask){
            break;
        }
    }
    result->num = i;
    result->bytes = malloc(sizeof(char) * i);
    for (i = result->num - 1; i >= 0; --i){
        result->bytes[i] = ((value >> (8 *(i))) & 0xff);
    }

    return result;
}

unsigned long long getNumber(NcNumber *self){
    unsigned long long result = 0;
    int i;
    for (i = self->num - 1; i >= 0; --i){
        result |= (self->bytes[i] << (8*i));
    }
    return result;
}

int main(int argc, char *argv[]){
    unsigned long long value = 400000000;
/*    printf("%d\n", sizeof(unsigned long long));
    int num = 0;

    int i;
    for (i = 0; i < sizeof(unsigned long long); i++){ //use --i to be faster, then can break at first one
        if ( ((value >> (8 * i)) & 0xff) | mask != mask){
            num = i;
        }
    }
    printf("%d\n", num);*/

    NcNumber *data = newNumber(value);
    printf("%lu\n", getNumber(data));
}
