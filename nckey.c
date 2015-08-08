#include "nckey.h"
#include <stdlib.h>
#include <limits.h>

static unsigned char MASK = UCHAR_MAX;

unsigned char *newGeoKey(unsigned long long x, unsigned long long y, unsigned char z){
    unsigned char *result;

    int numBytes = (z + 7) / CHAR_BIT;
    result = malloc(sizeof(char) * ((numBytes * 2) + 1));

    int i;
    for (i = numBytes - 1; i >= 0; --i){
        result[1 + i] = ((x >> (CHAR_BIT * i)) & MASK);
        result[1 + i + numBytes] = ((y >> (CHAR_BIT * i)) & MASK);
    }
    result[0] = z;

    return result;
}

unsigned char *newCatKey(unsigned long long category){
    unsigned char *result;

    char i, numBytes; //unsigned causes failure later

    for (i = sizeof(unsigned long long); i > 0; --i){
        if (((category >> (CHAR_BIT *(i-1))) & 0xff) | MASK != MASK){
            break;
        }
    }
    numBytes = i;

    result = malloc(sizeof(char) * (i + 1));
    result[0] = i;
    for (i = numBytes - 1; i >= 0; --i){
        result[1 + i] = ((category >> (CHAR_BIT * i)) & 0xff);
    }

    return result;
}

unsigned char decodeGeoKey(unsigned char *self, unsigned long long *x, unsigned long long *y){
    *x = 0;
    *y = 0;
    unsigned char z = self[0];

    int numBytes = (z + 7) / CHAR_BIT;
    int i;
    for (i = numBytes - 1; i >= 0; --i){
        *x |= (self[1 + i] << (CHAR_BIT * i));
        *y |= (self[1 + i + numBytes] << (CHAR_BIT * i));
    }

    return z;
}

unsigned long long decodeCatKey(unsigned char *self){
    unsigned long long result = 0;
    unsigned char numBytes = self[0];

    int i;
    for (i = numBytes - 1; i >= 0; --i){
        result |= (self[1 + i] << (CHAR_BIT * i));
    }

    return result;
}

