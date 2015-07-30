#ifndef NC_QUERY_H
#define NC_QUERY_H

#include "nanocube.h"
#include "ncdata.h"

typedef struct CatConstraint {
    int *categories;
    int num;
} CatConstraint;

typedef struct TimeConstraint {
    int start;
    int binSize;
    int numBins;
} TimeConstraint;

typedef struct NcQuery {
    NcDataType type;
    union {
        GeoData *geo;
        CatConstraint *cat;
        TimeConstraint *time;
    } data;
    struct NcQuery *next;
    int drilldown; //array
} NcQuery;

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
void addCatConstraint(NcQuery *self, int dim, CatConstraint *cc, int drilldown);
void addTimeConstraint(NcQuery *self, int dim, TimeConstraint *tc, int drilldown);
NcResult *query(Nanocube *nc, NcQuery *query);
void printResult(NcResult *self, int depth);

void tile(Nanocube *nc, NcQuery *query);
TileData *tileDrillDown(NcNode *root, int x, int y, int z);

NcResult *geoQuery(NcQuery *self, NcNode *root);
NcResult *catQuery(NcQuery *self, NcNode *root);
NcResult *timeQuery(NcQuery *self, NcNode *root);
NcResult *newResult();
NcResult *rollupResults(NcResult **results, int num);

#endif
