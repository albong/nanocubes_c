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
    result->type = GEO;
    result->data.geo = NULL;
    result->drilldown = 0;

    NcQuery *curr = result;
    int i;
    for (i = 1; i < (nc->numSpatialDim + nc->numCategories); i++){
        curr->next = malloc(sizeof(NcQuery));
        curr = curr->next;
        curr->drilldown = 0;
        if (i < nc->numSpatialDim){
            curr->type = GEO;
            curr->data.geo = NULL;
        } else {
            curr->type = CAT;
            curr->data.cat = NULL;
        }
    }
    curr->next = malloc(sizeof(NcQuery));
    curr = curr->next;
    curr->drilldown = 0;
    curr->type = TIME;
    curr->data.time = NULL;

    return result;
}

void addGeoConstraint(NcQuery *self, int dim, GeoData *gd, int drilldown){
    NcQuery *curr = self;
    int i;
    for (i = 0; i < dim; i++){
        curr = curr->next;
    }
    curr->data.geo = malloc(sizeof(GeoData));
    memcpy(curr->data.geo, gd, sizeof(GeoData));
    curr->drilldown = drilldown;
}

void addCatConstraint(NcQuery *self, int dim, CatConstraint *cc, int drilldown){
    NcQuery *curr = self;
    int i;
    for (i = 0; i < dim; i++){
        curr = curr->next;
    }
    curr->data.cat = malloc(sizeof(CatConstraint));
    memcpy(curr->data.cat, cc, sizeof(CatConstraint));
    curr->drilldown = drilldown;
}

void addTimeConstraint(NcQuery *self, int dim, TimeConstraint *tc, int drilldown){
    NcQuery *curr = self;
    int i;
    for (i = 0; i < dim; i++){
        curr = curr->next;
    }
    curr->data.time = malloc(sizeof(TimeConstraint));
    memcpy(curr->data.time, tc, sizeof(TimeConstraint));
    curr->drilldown = drilldown;
}

NcResult *query(Nanocube *nc, NcQuery *query){
    return geoQuery(query, nc->root);
}

void printResult(NcResult *self, int depth){
    int i;
    for (i = 0; i < depth; i++){
        printf(" ");
    }
    printf("%d,%d | ", self->addr.dates.start, self->addr.dates.end);
    printf("%lu | %p\n", self->count, self->children);
    if (self->children != NULL){
        for (i = 0; i < self->count; i++){
            printResult(self->children[i], depth+1);
        }
    }
}

NcResult *geoQuery(NcQuery *self, NcNode *root){
/*    int i;
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
*/
    NcResult *result;
    GeoData *gd = self->data.geo;
    NcNode *node;
    int nextX, nextY;
    int i;
    GeoNode *gn;
    NcNode *curr, *next;

    if (self->drilldown == 1){
        if (gd == NULL){
            //return top level tile
            printf("return top tile\n");
        } else {
            //return specific tile
            printf("return specific tile\n");
        }
    } else {
        if (gd == NULL){
            //no constraint, skip
            printf("no geo constraint\n");
            node = root;
        } else {
            //return counts for node
            printf("get counts for geo\n");
            curr = root;
            gn = (GeoNode *)curr->node;
            while(gn->z > gd->z){
                next = NULL;
                nextX = gd->x / (1 << (MAX_GEO_DEPTH - (gn->z+1)));
                nextY = gd->y / (1 << (MAX_GEO_DEPTH - (gn->z+1)));
                for (i = 0; i < curr->numChildren; i++){
                    gn = (GeoNode *)curr->children[i]->node;
                    if (gn->x == nextX && gn->y == nextY){
                        next = curr->children[i];
                        break;
                    }
                }
                if (next == NULL){
                    /*
                    NO DATA FOUND #PANIC
                    */
                    return newResult();
                } else {
                    curr = next;
                    gn = (GeoNode *)curr->node;
                }
            }
            node = curr;            
        }
        
        if (self->next->type == GEO){
            result = geoQuery(self->next, node->content);
        } else if (self->next->type == CAT){
            result = catQuery(self->next, node->content);
        } else { //time
            result = timeQuery(self->next, node->content);
        }
    }

    return result;
}

NcResult *catQuery(NcQuery *self, NcNode *root){
    int i, j;
    int drillCat;
    NcResult *result;
    CatConstraint *cc = self->data.cat;
    ConNode *cn;
    NcResult **toRoll;
    int numToRoll;
    if (self->drilldown == 1){
        result = newResult();
        if(cc == NULL){
            //return new result where each child has next query level
            printf("drilldown all cats\n");
        } else {
            //same as before but only for the specified children
            printf("drilldown special cats\n");
        }

        //loop over all children, if we're constrained then check against the list
        for (i = 0; i < root->numChildren; i++){
            cn = (ConNode *)root->children[i]->node;
            if (cc != NULL){
                for (j = 0; j < cc->num; j++){
                    if (cn->category == cc->categories[i]){
                        drillCat = 1;
                        break;
                    }
                }
            } else {
                drillCat = 1;
            }

            if (drillCat){
                result->count++;
                result->children = realloc(result->children, sizeof(NcResult) * result->count);
                if (self->next->type == CAT){
                    result->children[result->count - 1] = catQuery(self->next, root->content);
                } else { //time
                    result->children[result->count - 1] = timeQuery(self->next, root->content);
                }
            }
            drillCat = 0;
        }

    } else {
        if(cc == NULL){
            //no constraint, skip
            printf("no cat constraints\n");
            if (self->next->type == CAT){
                result = catQuery(self->next, root->content);
            } else { //time
                result = timeQuery(self->next, root->content);
            }
        } else {
            //return rolled up time data for each category
            printf("rollup special cats\n");
            toRoll = NULL;
            numToRoll = 0;

            for (i = 0; i < root->numChildren; i++){
                cn = (ConNode *)root->children[i]->node;
                for (j = 0; j < cc->num; j++){
                    if (cn->category == cc->categories[i]){
                        numToRoll++;
                        toRoll = realloc(toRoll, sizeof(NcResult *) * numToRoll);
                        if (self->next->type == CAT){
                            toRoll[numToRoll - 1] = catQuery(self->next, root->content);
                        } else { //time
                            toRoll[numToRoll - 1] = timeQuery(self->next, root->content);
                        }
                        toRoll[numToRoll - 1]->addr.dates.start = cn->category;
                    }
                }
            }
            result = rollupResults(self, toRoll, numToRoll);
        }
    }

    return result;
}

NcResult *timeQuery(NcQuery *self, NcNode *root){
    //if constraint is null and no drilldown, then nothing, otherwise there are cases
    TimeConstraint *tc = self->data.time;
    TimeNode *tn = (TimeNode *)root->node;
    NcResult *result = newResult();

    int numBins;
    int binSize;
    int start;
    int end;
    if (self->drilldown){
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
        result->children[currBin]->count += getCountAtTime(tn->timeseries, i);
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
    result->addr.dates.start = 0;
    result->addr.dates.end = 0;
    return result;
}

NcResult *rollupResults(NcQuery *query, NcResult **results, int num){
    NcResult *rolled = newResult();
 
    NcResult **toRoll = NULL;
    int numToRoll = 0;
    int i, j, k, l;
    int currStart, currEnd;
    for (i = 0; i < num; i++){
        if (results[i]->children != NULL){
            for (j = 0; j < results[i]->count; j++){
                if (results[i]->children[j] == NULL){
                    continue;
                }
                numToRoll++;
                toRoll = realloc(toRoll, sizeof(NcResult) * numToRoll);
                toRoll[numToRoll-1] = results[i]->children[j];
                currStart = results[i]->children[j]->addr.dates.start;
                currEnd = results[i]->children[j]->addr.dates.end;
                results[i]->children[j] = NULL;

                for (k = i+1; k < num; k++){
                    for (l = 0; l < results[k]->count; l++){
                        if (results[k]->children[l] == NULL){
                            continue;
                        } else if (results[i]->children[j]->addr.dates.start == results[k]->children[l]->addr.dates.start &&
                                    results[i]->children[j]->addr.dates.end == results[k]->children[l]->addr.dates.end) {//if the i-j addr matches the k-l addr, add to list
                            numToRoll++;
                            toRoll = realloc(toRoll, sizeof(NcResult) * numToRoll);
                            toRoll[numToRoll-1] = results[k]->children[l];
                            results[k]->children[l] = NULL;
                        }
                    }
                }
                
                if (numToRoll > 0){
                    rolled->count++;
                    rolled->children = realloc(rolled->children, rolled->count * sizeof(NcResult *));
                    rolled->children[rolled->count - 1] = rollupResults(query->next, toRoll, numToRoll);
                    rolled->children[rolled->count - 1]->addr.dates.start = currStart;
                    rolled->children[rolled->count - 1]->addr.dates.end = currEnd;
                    numToRoll = 0;
                    toRoll = NULL;
                }

            }
            /*
            No freeing happening, need someway to mark child as already rolled,
            if we free and check if null that could work
            write a similar method that recursively frees, pass in the toRoll and numToRoll
            */
        } else {
            rolled->addr.dates.start = results[i]->addr.dates.start;//these addresses should all be same
            rolled->addr.dates.end = results[i]->addr.dates.end;
            rolled->count += results[i]->count;
        }
    }
    
    return rolled;
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

