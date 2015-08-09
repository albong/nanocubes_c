#ifndef NC_NODE_H
#define NC_NODE_H

#include "timeseries.h"
#include "ncdata.h"
#include "nckey.h"

#define MAX_GEO_DEPTH 5

typedef struct NcNode {
    unsigned char *key;
    union {
        struct NcNode *node;
        Timeseries *timeseries;
    } content;
    unsigned char *linkShared;
    int sharedContent;
    struct NcNode **children; //array
    int numChildren; //can we optimize by having a GeoNode that only has four children, and a CatNode with a char *?
} NcNode;

typedef struct NcNodeStack {
    NcNode *node;
    struct NcNodeStack *next;
} NcNodeStack;

NcNode *newNcNode(NcDataType type);
NcNode *newGeoNode(int x, int y, int z);
NcNode *newCatNode(int category);

NcNodeStack *newNcNodeStack();
void push(NcNodeStack *self, NcNode *node);
NcNode *pop(NcNodeStack *self);
int stackEmpty(NcNodeStack *self);

int getMatchingChildInd(NcNode *self, NcValueChain *values, int index, NcDataType type);

NcNode *newProperChild(NcNode *self, NcValueChain *values, int index);
NcNode *replaceChild(NcNode *self, int childInd);
NcNode *shallowCopyNode(NcNode *self); //investigate where this is used from, may be only for timeseries

int nodeInList(NcNode *self, NcNode **list, size_t size);

void insertData(NcNode *self, int dim, int timeDim, NcData *data);

int checkShared(unsigned char *self, size_t index);
unsigned char *setShared(unsigned char *self, size_t index, int shared);

#endif
