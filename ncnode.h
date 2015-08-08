#ifndef NC_NODE_H
#define NC_NODE_H

#include "timeseries.h"
#include "ncdata.h"
#include "nckey.h"

#define MAX_GEO_DEPTH 5

typedef struct NcNode {
    union {
        GeoKey geo; //instead of a struct, have a char * - less memory due to alignment
        CatKey cat;
    } key;
    struct NcNode **children; //array
    int numChildren;
    int *isShared; //array - rename isChildShared - replace with char *, put sharedContent here too
    union {
        struct NcNode *node;
        Timeseries *timeseries;
    } content;
    int sharedContent;
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

#endif
