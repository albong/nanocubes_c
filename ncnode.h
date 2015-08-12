#ifndef NC_NODE_H
#define NC_NODE_H

#include "timeseries.h"
#include "ncdata.h"
#include "nckey.h"

typedef struct NcNode {
    unsigned char *key;
    union {
        struct NcNode *node;
        Timeseries *timeseries;
    } content;
    unsigned char *linkShared;//bitfield, 0 is content, i is child i-1
    struct NcNode **children; //array
    size_t numChildren; //sizeof(size_t) should be <= sizeof(<pointer>), no extra padding
} NcNode;

typedef struct CatNode {
    unsigned char *key;
    union {
        NcNode *node;
        Timeseries *timeseries;
    } content;
    unsigned char *linkShared;//bitfield, 0 is content, i is child i-1
} CatNode;

typedef struct NcNodeStack {
    NcNode *node;
    struct NcNodeStack *next;
} NcNodeStack;

NcNode *newNcNode(NcDataType type);
NcNode *newGeoNode(unsigned long long x, unsigned long long y, unsigned char z); //need to replace all of these with unsigned long longs
NcNode *newCatNode(unsigned long long category);
NcNode *newChildlessCatNode(unsigned long long category);

NcNodeStack *newNcNodeStack();
void push(NcNodeStack *self, NcNode *node);
NcNode *pop(NcNodeStack *self);
int stackEmpty(NcNodeStack *self);
void freeStack(NcNodeStack *self);

int getMatchingChildInd(NcNode *self, NcValueChain *values, size_t index, NcDataType type);

NcNode *newProperChild(NcNode *self, NcValueChain *values, size_t index);
NcNode *newProperCatChild(NcNode *self, NcValueChain *values, size_t index);
NcNode *replaceChild(NcNode *self, size_t childInd, int childIsCat);
NcNode *shallowCopyNode(NcNode *self); //investigate where this is used from, may be only for timeseries
NcNode *shallowCopyCatNode(CatNode *self);

int nodeInList(NcNode *self, NcNode **list, size_t size);

void insertData(NcNode *self, int dim, int timeDim, NcData *data);

int checkShared(unsigned char *self, size_t index);
unsigned char *setShared(unsigned char *self, size_t index, int shared);

#endif
