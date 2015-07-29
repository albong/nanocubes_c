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

typedef struct TimeResult {//replace this with NcResult
    int num;
    int *start;//array
    int *end;//array
    unsigned long long *count;//array
} TimeResult;

typedef struct NcResult {
    struct NcResult **children; //array - can we union this too?
//    unsigned int addr; //replace this with pair of size_t's?
    union {
        int category;
        struct {
            int start;
            int end;
        } dates;
    } addr;
    unsigned long long count;
} NcResult;

NcQuery *newQuery(Nanocube *nc);
void addGeoConstraint(NcQuery *self, int dim, GeoData *gd, int drilldown);
void addCatConstraint(NcQuery *self, int dim, CatData *cd, int drilldown);
void addTimeConstraint(NcQuery *self, int dim, TimeConstraint *tc, int drilldown);
void query(Nanocube *nc, NcQuery *query);

void tile(Nanocube *nc, NcQuery *query);
TileData *tileDrillDown(NcNode *root, int x, int y, int z);

void geoQuery(NcQuery *self, int currDim, NcNode *root);
NcResult *catQuery(NcQuery *self, int currDim, NcNode *root);
TimeResult *rollupTime(TimeResult **results);
NcResult *timeQuery(NcQuery *self, int currDim, NcNode *root);
NcResult *newResult();


#endif
