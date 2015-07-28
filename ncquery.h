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

typedef struct TileData {
    int num;
    int *x;
    int *y;
    unsigned long long *count;
} TileData;

NcQuery *newQuery(Nanocube *nc);
void addGeoConstraint(NcQuery *self, int dim, GeoData *gd, int drilldown);
void tile(Nanocube *nc, NcQuery *query);
TileData *tileDrillDown(NcNode *root, int x, int y, int z);

void geoQuery(NcQuery *self, int currDim, NcNode *root);
void catQuery(NcQuery *self, int currDim, NcNode *root);
void timeQuery(NcQuery *self, int currDim, NcNode *root);



#endif