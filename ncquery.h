#ifndef NC_QUERY_H
#define NC_QUERY_H

#include "nanocube.h"
#include "ncdata.h"

typedef struct NcQuery {
    NcDataType *type;
    NcData *data;
    struct NcQuery *next;
    int *drilldown; //array
} NcQuery;

typedef struct TimeConstraint {
    int start;
    int binSize;
    int numBins;
} TimeConstraint;

typedef struct TileData { //rename TileResult
    int num;
    int *x;
    int *y;
    unsigned long long *count;
} TileData;

typedef struct TimeResult {
    int num;
    int *start;//array
    int *end;//array
    unsigned long long *count;//array
} TimeResult;

typedef struct CatResult {
    int num;
    int *category; //array
    TimeResult **tResult; //array of pointers
    struct CatResult **cResult; //array of pointers
} CatResult;

NcQuery *newQuery(Nanocube *nc);
void addGeoConstraint(NcQuery *self, int dim, GeoData *gd, int drilldown);
void addCatConstraint(NcQuery *self, int dim, CatData *cd, int drilldown);
void addTimeConstraint(NcQuery *self, int dim, TimeConstraint *tc, int drilldown);
void query(Nanocube *nc, NcQuery *query);

void tile(Nanocube *nc, NcQuery *query);
TileData *tileDrillDown(NcNode *root, int x, int y, int z);

void geoQuery(NcQuery *self, int currDim, NcNode *root);
void catQuery(NcQuery *self, int currDim, NcNode *root);
TimeResult *timeQuery(NcQuery *self, int currDim, NcNode *root);



#endif
