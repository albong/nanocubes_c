#include "ncquery.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ncnode.h"

#define RESOLUTION 7

static TileData *tileDrilldown(NcNode *root, int x, int y, int z);
static void findPoints(NcNode *self, TileData *td, int xOffset, int yOffset, int deepest);
static void addData(TileData *self, int x, int y, unsigned long long count);

NcQuery *newQuery(Nanocube *nc){
    NcQuery *result = malloc(sizeof(NcQuery));
    result->data = malloc(sizeof(NcData));
    result->data->data = NULL;
    result->type = malloc(sizeof(NcDataType) * (nc->numSpatialDim + nc->numCategories + 1));

    NcData *curr = result->data;
    int i;
    for (i = 1; i < (nc->numSpatialDim + nc->numCategories); i++){
        curr->next = malloc(sizeof(NcData));
        curr = curr->next;
        curr->data = NULL;

        if (i < nc->numSpatialDim){
            result->type[i] = GEO;
        } else {
            result->type[i] = CAT;
        }
    }
    result->type[i] = TIME;
    //we've covered the geo and cat dimensions, now we need to cover the time dimension
    curr->next = malloc(sizeof(NcQuery));
    curr = curr->next;
    curr->data = NULL;

    result->drilldown = malloc(sizeof(int) * (nc->numSpatialDim + nc->numCategories + 1));
    for (i = 0; i < (nc->numSpatialDim + nc->numCategories + 1); i++){
        result->drilldown[i] = 0;
    }
    return result;
}

void addGeoConstraint(NcQuery *self, int dim, GeoData *gd, int drilldown){
    NcData *constraint = getDataAtInd(self->data, dim);
    constraint->data = malloc(sizeof(GeoData));
    memcpy(constraint->data, gd, sizeof(GeoData));
    self->drilldown[dim] = drilldown;
}

void addCatConstraint(NcQuery *self, int dim, CatData *cd, int drilldown){
    NcData *constraint = getDataAtInd(self->data, dim);
    constraint->data = malloc(sizeof(CatData));
    memcpy(constraint->data, cd, sizeof(CatData));
    self->drilldown[dim] = drilldown;
}

void addTimeConstraint(NcQuery *self, int dim, TimeConstraint *tc, int drilldown){
    NcData *constraint = getDataAtInd(self->data, dim);
    constraint->data = malloc(sizeof(TimeConstraint));
    memcpy(constraint->data, tc, sizeof(TimeConstraint));
    self->drilldown[dim] = drilldown;
}

void query(Nanocube *nc, NcQuery *query){
    geoQuery(query, 0, nc->root);
}

void geoQuery(NcQuery *self, int currDim, NcNode *root){
    int i;
    int nextX, nextY;
    GeoNode *gn;
    GeoData *gdConstraint;
    NcNode *curr, *next;
    NcData *constraint = getDataAtInd(self->data, currDim);

    if (constraint->data == NULL){
        printf("geo - no constraint\n");
        if (self->type[currDim+1] == GEO){
            geoQuery(self, currDim+1, root->content);
        } else if (self->type[currDim+1] == CAT){
            catQuery(self, currDim+1, root->content);
        } else { //time
            timeQuery(self, currDim+1, root->content);
        }
    } else if (self->drilldown[currDim] == 1){
        printf("geo - run tile\n");
        //tile       
    } else {
        printf("geo - rollup\n");
        //find the correct child and continue - does not handle region queries
        gdConstraint = (GeoData *)constraint->data;
        curr = root;
        gn = (GeoNode *)curr->node;
        while(gn->z > gdConstraint->z){
            next = NULL;
            nextX = gdConstraint->x / (1 << (MAX_GEO_DEPTH - (gn->z+1)));
            nextY = gdConstraint->y / (1 << (MAX_GEO_DEPTH - (gn->z+1)));
            for (i = 0; i < curr->numChildren; i++){
                gn = (GeoNode *)curr->children[i]->node;
                if (gn->x == nextX && gn->y == nextY){
                    next = curr->children[i];
                    break;
                }
            }
            if (next == NULL){
                printf("0\n");
                return;
            } else {
                curr = next;
                gn = (GeoNode *)curr->node;
            }
        }

        //if no child, then return 0;
        if (self->type[currDim+1] == GEO){
            geoQuery(self, currDim+1, curr->content);
        } else if (self->type[currDim+1] == CAT){
            catQuery(self, currDim+1, curr->content);
        } else { //time
            timeQuery(self, currDim+1, curr->content);
        }
    }
}

NcResult *catQuery(NcQuery *self, int currDim, NcNode *root){
    int i;
    ConNode *cn;
    CatData *cdConstraint;
    NcNode *next;
    NcData *constraint = getDataAtInd(self->data, currDim);
 /*   
    if (constraint->data == NULL){
        printf("cat - no constraint\n");
        if (self->type[currDim+1] == GEO){
            geoQuery(self, currDim+1, root->content);
        } else if (self->type[currDim+1] == CAT){
            catQuery(self, currDim+1, root->content);
        } else { //time
            timeQuery(self, currDim+1, root->content);
        }
    } else if (self->drilldown[currDim] == 1){
        //print for each category - need rollup
        printf("cat - break out by category\n");
    } else {
        printf("cat - select from category\n");
        cdConstraint = (CatData *)constraint->data;
        next = NULL;
        for (i = 0; i < root->numChildren; i++){
            cn = (ConNode *)root->children[i]->node;
            if (cn->category == cdConstraint->category){
                next = root->children[i];
                break;
            }
        }

        if (next == NULL){
            printf("0\n");
        } else if (self->type[currDim+1] == CAT){
            catQuery(self, currDim+1, next->content);
        } else { //time
            timeQuery(self, currDim+1, next->content);
        }
    }
*/

    NcResult *result;
    if (self->drilldown[currDim] == 1){
        if(constraint->data == NULL){
            //return new result where each child has next query level
        } else {
            //same as before but only for the specified children
        }
    } else {
        if(constraint->data == NULL){
            //no constraint, skip
            if (self->type[currDim+1] == CAT){
                result = catQuery(self, currDim+1, root->content);
            } else { //time
                //ought just be returing the time thingy, assuming that we've hence converted the
                //type to return the other kind of struct
                result = malloc(sizeof(NcResult));
                result->addr = 0;
                result->children = NULL;
                result->data = timeQuery(self, currDim+1, root->content);
            }
        } else {
            //return rolled up time data for each category
        }
    }

    return result;
}

TimeResult *rollupTime(TimeResult **results){
    //make new and free the old
}

NcResult *timeQuery(NcQuery *self, int currDim, NcNode *root){
    //if constraint is null and no drilldown, then nothing, otherwise there are cases
    NcData *constraint = getDataAtInd(self->data, currDim);
    TimeConstraint *tc = (TimeConstraint *)constraint->data;
    TimeNode * tn = (TimeNode *)root->node;
    NcResult *result = newResult();

    int numBins;
    int binSize;
    int start;
    int end;
    if (self->drilldown[currDim]){
        //return timeseries
        printf("time - return series\n");
        if (tc != NULL){
            //only bin as in the constraint
            start = tc->start;
            end = tc->start + (tc->binSize * tc->numBins) - 1;
            binSize = tc->binSize;
            numBins = tc->numBins;
        } else {
            //bin size is 1
            start = 0;
            end = tn->timeseries->numBins - 1;
            binSize = 1;
            numBins = tn->timeseries->numBins;
        }
    } else {
        //sum data across this time
        printf("time - return total\n");
        if (tc != NULL){
            //total in range
            start = tc->start;
            end = tc->start + (tc->binSize * tc->numBins) - 1;
            binSize = end - start + 1;
            numBins = 1;
        } else {
            //total overall
            start = 0;
            end = tn->timeseries->numBins - 1;
            binSize = tn->timeseries->numBins;
            numBins = 1;
        }
    }
    
    result->count = numBins;
    result->children = malloc(numBins * sizeof(NcResult *));

    int i;
    int count = 0;
    int currBin = 0;
    for (i = start; i <= end; i++){
        if (count == 0){
            result->children[currBin] = newResult();
            result->children[currBin]->addr.dates.start = i;
        }
        result->children[i]->count += getCountAtTime(tn->timeseries, i);
        count++;
        if (count == binSize){
            result->children[currBin]->addr.dates.end = i;
            currBin++;
            count = 0;
        }
    }

    printf("[");
    for (i = 0; i < result->count; i++){
        printf("%d,%d,%lu ", result->children[i]->addr.dates.start, result->children[i]->addr.dates.end, result->children[i]->count);
    }
    printf("]\n");

    return result;
}

NcResult *newResult(){
    NcResult *result = malloc(sizeof(NcResult));
    result->children = NULL;
    result->count = 0;
    result->addr.category = 0;
    return result;
}



void tile(Nanocube *nc, NcQuery *query){
    NcNode *curr = nc->root;
    //tileDrillDown(curr, x, y, z);
    int i;
    NcData *constraint;
    for (i = 0; i < nc->numSpatialDim; i++){
        
    }
}

TileData *tileDrillDown(NcNode *root, int x, int y, int z){
    GeoData *gd = (GeoData *)root->node;
    int nextX, nextY;
    int i;
    TileData *result = NULL;
    
    //do we need a case for if we've gone too deep?
    if (gd->x == x && gd->y == y && gd->z == z){
        result = malloc(sizeof(TileData));
        result->num = 0;
        result->x = NULL;
        result->y = NULL;
        result->count = NULL;
        findPoints(root, result, gd->x * (1 << RESOLUTION), gd->y * (1 << RESOLUTION), gd->z + RESOLUTION);
    } else {
        nextX = x / (1 << (MAX_GEO_DEPTH - (gd->z+1)));
        nextY = y / (1 << (MAX_GEO_DEPTH - (gd->z+1)));
        for (i = 0; i < root->numChildren; i++){
            gd = (GeoData *)root->children[i]->node;
            if (gd->x == nextX && gd->y == nextY){
                result = tileDrillDown(root->children[i], x, y, z);
                break;
            }
        }
    }

    return result;
}

void findPoints(NcNode *self, TileData *td, int xOffset, int yOffset, int deepest){
    GeoData *gd = self->node;
    int i;
    int x, y;
    unsigned long long count;

    if (gd->z == deepest){
//        count = getCount(self->content);
        x = gd->x - xOffset;
        y = gd->y - yOffset;
        addData(td, x, y, count);
    } else {
        for (i = 0; i < self->numChildren; i++){
            findPoints(self->children[i], td, xOffset, yOffset, deepest);
        }
    }
}

void addData(TileData *self, int x, int y, unsigned long long count){
    self->num++;
    self->x = realloc(self->x, sizeof(int) * self->num);
    self->y = realloc(self->y, sizeof(int) * self->num);
    self->count = realloc(self->count, sizeof(unsigned long long) * self->num);
    self->x[self->num-1] = x;
    self->y[self->num-1] = y;
    self->count[self->num-1] = count;
}

