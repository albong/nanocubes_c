#ifndef NANOCUBE_H
#define NANOCUBE_H
#include <stddef.h>
#include "ncnode.h"
#include "ncdata.h"

typedef struct Nanocube {
    NcNode *root;
    NcDataType *dimensions; //array - can we remove this and just use the below?
    size_t numSpatialDim;
    size_t numCategoricalDim;
} Nanocube;

Nanocube *newNanocube(size_t numSpatialDim, size_t numCategoricalDim);
void addToNanocube(Nanocube *nc, size_t time, unsigned long long count, ...);
void addArraysToNanocube(Nanocube *nc, unsigned long long *x, unsigned long long *y, unsigned long long *cat, size_t time, unsigned long long count);
void printNanocube(Nanocube *self);
void printNode(NcNode *self, int padding, int isShared, int isContent, Nanocube *nc, int dim);
void printStack(NcNodeStack *self, Nanocube *nc, int dim);

NcNodeStack *trailProperPath(Nanocube *nc, NcNode *root, NcValueChain *values, int dim);

#endif
