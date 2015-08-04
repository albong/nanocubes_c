#ifndef NC_KEY_H
#define NC_KEY_H

typedef struct GeoKey {
    char *xy;
    unsigned char z;
} GeoKey;

typedef struct CatKey {
    char *category;
    unsigned char numBytes;
} CatKey;

GeoKey *newGeoKey(unsigned long long x, unsigned long long y, unsigned char z);
CatKey *newCatKey(unsigned long long category);

unsigned char decodeGeoKey(GeoKey *self, unsigned long long *x, unsigned long long *y);
unsigned long long decodeCatKey(CatKey *self);

#endif
