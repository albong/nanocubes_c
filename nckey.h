#ifndef NC_KEY_H
#define NC_KEY_H

//merge all of this into ncnode.h

typedef struct GeoKey {
    char *xy;
    unsigned char z;
} GeoKey;

typedef struct CatKey {
    char *category;
    unsigned char numBytes;
} CatKey;

unsigned char *newGeoKey(unsigned long long x, unsigned long long y, unsigned char z);
unsigned char *newCatKey(unsigned long long category);

unsigned char decodeGeoKey(unsigned char *self, unsigned long long *x, unsigned long long *y);
unsigned long long decodeCatKey(unsigned char *self);

#endif
