#ifndef NANOCUBE_H
#define NANOCUBE_H
#include <stddef.h>
#include "ncnode.h"
#include "ncdata.h"

typedef struct Nanocube {
    size_t numSpatialDim;
    size_t numCategories;//rename to numCategoricalDim
    int *categories; //do we even use this?  I doubt it
    NcNode *root;

    //do we need to have these and the above counts?
    NcDataType *dimensions; //array
    size_t numDim;
} Nanocube;

Nanocube *newNanocube(size_t numSpatialDim, size_t numCategories);
void addToNanocube(Nanocube *nc, int x, int y, int time, int cat);
void printNanocube(Nanocube *self);

#endif