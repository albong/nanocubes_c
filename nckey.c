#include "nckey.h"
#include <stdlib.h>
#include <limits.h>

static unsigned char MASK = UCHAR_MAX;

GeoKey *newGeoKey(unsigned long long x, unsigned long long y, unsigned char z){
    GeoKey *result = malloc(sizeof(GeoKey));

    result->z = z;    
    int numBytes = (z / CHAR_BIT) + 1;
    result->xy = malloc(sizeof(char) * numBytes * 2);
 
    int i;
    for (i = numBytes - 1; i >= 0; --i){
        result->xy[i] = ((x >> (CHAR_BIT * i)) & MASK);
        result->xy[i + numBytes] = ((y >> (CHAR_BIT * i)) & MASK);
    }

    return result;
}

CatKey *newCatKey(unsigned long long category){
    CatKey *result = malloc(sizeof(CatKey));

    int i;
    for (i = sizeof(unsigned long long); i > 0; --i){
        if (((category >> (CHAR_BIT *(i-1))) & 0xff) | MASK != MASK){
            break;
        }
    }
    result->numBytes = i;
    result->category = malloc(sizeof(char) * i);
    for (i = result->numBytes - 1; i >= 0; --i){
        result->category[i] = ((category >> (CHAR_BIT * i)) & 0xff);
    }

    return result;
}

unsigned char decodeGeoKey(GeoKey *self, unsigned long long *x, unsigned long long *y){
    *x = 0;
    *y = 0;

    int numBytes = (self->z / CHAR_BIT) + 1;
    int i;
    for (i = numBytes - 1; i >= 0; --i){
        *x |= (self->xy[i] << (CHAR_BIT * i));
        *y |= (self->xy[i + numBytes] << (CHAR_BIT * i));
    }

    return self->z;
}

unsigned long long decodeCatKey(CatKey *self){
    unsigned long long result = 0;

    int i;
    for (i = self->numBytes - 1; i >= 0; --i){
        result |= (self->category[i] << (CHAR_BIT * i));
    }

    return result;
}

