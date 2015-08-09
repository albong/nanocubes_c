#ifndef NC_KEY_H
#define NC_KEY_H

//merge all of this into ncnode.h

unsigned char *newGeoKey(unsigned long long x, unsigned long long y, unsigned char z);
unsigned char *newCatKey(unsigned long long category);

unsigned char decodeGeoKey(unsigned char *self, unsigned long long *x, unsigned long long *y);
unsigned long long decodeCatKey(unsigned char *self);

#endif
