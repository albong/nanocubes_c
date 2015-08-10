#ifndef NANOCUBE_H
#define NANOCUBE_H
#include <stddef.h>
#include "ncnode.h"
#include "ncdata.h"

typedef struct Nanocube {
    size_t numSpatialDim;
    size_t numCategoricalDim;//rename to numCategoricalDim
    int *categories; //do we even use this?  I doubt it
    NcNode *root;

    //do we need to have these and the above counts?
    NcDataType *dimensions; //array
} Nanocube;

Nanocube *newNanocube(size_t numSpatialDim, size_t numCategoricalDim);
void addToNanocube(Nanocube *nc, int time, unsigned long long count, ...);
void printNanocube(Nanocube *self);
void printNode(NcNode *self, int padding, int isShared, int isContent, Nanocube *nc, int dim);
void printStack(NcNodeStack *self, Nanocube *nc, int dim);

NcNodeStack *trailProperPath(Nanocube *nc, NcNode *root, NcValueChain *values, int dim);

#endif
