#ifndef NC_NODE_H
#define NC_NODE_H

#include "timeseries.h"
#include "ncdata.h"

#define MAX_GEO_DEPTH 5

typedef struct NcNode {
    NcDataType type;
    void *node;
    struct NcNode **children; //array
    int numChildren;
    int *isShared; //array
    struct NcNode *content;
    int sharedContent;
} NcNode;

typedef struct GeoNode {
    int x;
    int y;
    int z;
} GeoNode;

typedef struct CatNode {
    int category;
} CatNode;

typedef struct TimeNode {
    Timeseries *timeseries;
} TimeNode;

typedef struct NcNodeStack {
    NcNode *node;
    struct NcNodeStack *next;
} NcNodeStack;

NcNode *newNcNode(NcDataType type);
NcNode *newGeoNode(int x, int y, int z);
NcNode *newCatNode(int category);
NcNode *newTimeNode();

//static GeoNode *makeGeoNode(int x, int y, int z);
//static CatNode *makeCatNode(int category);
//static TimeNode *makeTimeNode();

NcNodeStack *newNcNodeStack();
void push(NcNodeStack *self, NcNode *node);
NcNode *pop(NcNodeStack *self);
int stackEmpty(NcNodeStack *self);

NcNode *getMatchingChild(NcNode *self, NcValueChain *values, int index);
int getMatchingChildInd(NcNode *self, NcValueChain *values, int index);

NcNode *newProperChild(NcNode *self, NcValueChain *values, int index);
NcNode *replaceChild(NcNode *self, int childInd);
NcNode *shallowCopyNode(NcNode *self);

int nodeInList(NcNode *self, NcNode **list, size_t size);

void insert(NcNode *self, int time, unsigned long long count);
void insertData(NcNode *self, NcData *data);

#endif
