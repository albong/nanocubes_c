#include "nanocube.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#include "nckey.h"

static void addGeo(Nanocube *nc, NcNode *root, NcData *data, int dim, NcNode **updatedList, size_t *numUpdated);
static void addCat(Nanocube *nc, NcNode *root, NcData *data, int dim, NcNode **updatedList, size_t *numUpdated);

Nanocube *newNanocube(size_t numSpatialDim, size_t numCategoricalDim, int maxDepth){
    Nanocube *result = malloc(sizeof(Nanocube));
    int i;
    
    result->numSpatialDim = numSpatialDim;
    result->numCategoricalDim = numCategoricalDim;

    result->root = newGeoNode(0,0,0);

    result->dimensions = malloc(sizeof(NcDataType) * (numSpatialDim + numCategoricalDim));
    for (i = 0; i < (numSpatialDim + numCategoricalDim); i++){
        if (i < numSpatialDim){
            result->dimensions[i] = GEO;
        } else {
            result->dimensions[i] = CAT;
        }
    }

    result->dataCount = 0;
    result->memCount = 0;
    result->maxDepth = maxDepth;

    return result; 
}

void addToNanocube(Nanocube *nc, size_t time, unsigned long long count, ...){
    va_list input;
    int i;
    NcData *data;
    NcData *curr;

    va_start(input, count);
    int x = va_arg(input, int);
    int y = va_arg(input, int);
    data = newGeoData(x, y, nc->maxDepth);
    curr = data;
    for (i = 1; i < (nc->numSpatialDim + nc->numCategoricalDim); i++){//we always must have at least one spatial dim
        if (i < nc->numSpatialDim){
            x = va_arg(input, int);
            y = va_arg(input, int);
            curr->next = newGeoData(x, y, nc->maxDepth);
            curr = curr->next;
        } else {
            curr->next = newCatData(va_arg(input, int));
            curr = curr->next;
        }
    }
    va_end(input);
    curr->next = newTimeData(time, count);

/*    curr = data;
    i = 0;
    while (curr != NULL){
        if (i < nc->numSpatialDim){
            printf("Geo: %d,%d,%d\n", ((GeoData *)curr->data)->x, ((GeoData *)curr->data)->y, ((GeoData *)curr->data)->z);
        } else if (i < (nc->numSpatialDim + nc->numCategories)){
            printf("Cat: %d\n", ((CatData *)curr->data)->category);
        } else {
            printf("Time: %d,%lu\n", ((TimeData *)curr->data)->time, ((TimeData *)curr->data)->count);
        }
        i++;
        curr = curr->next;
    }
*/
    size_t numUpdated = 0;
    addGeo(nc, nc->root, data, 1, NULL, &numUpdated);//fix the dim shenangians to be 0 indexed?
    freeData(data);
    nc->dataCount++;
    nc->memCount += getMemCount();
}

void addArraysToNanocube(Nanocube *nc, unsigned long long *x, unsigned long long *y, unsigned long long *cat, size_t time, unsigned long long count){
    va_list input;
    int i;
    NcData *data;
    NcData *curr;

    data = newGeoData(x[0], y[0], nc->maxDepth);
    curr = data;

    //add the geodata
    for (i = 1; i < nc->numSpatialDim; i++){
        curr->next = newGeoData(x[i], y[i], nc->maxDepth);
        curr = curr->next;
    }

    //add the catdata
    for (i = 0; i < nc->numCategoricalDim; i++){
        curr->next = newCatData(cat[i]);
        curr = curr->next;
    }

    //add the time data
    curr->next = newTimeData(time, count);

    size_t numUpdated = 0;
    addGeo(nc, nc->root, data, 1, NULL, &numUpdated);//fix the dim shenangians to be 0 indexed?
    freeData(data);
    nc->dataCount++;
    nc->memCount += getMemCount();
}

void addGeo(Nanocube *nc, NcNode *root, NcData *data, int dim, NcNode **updatedList, size_t *numUpdated){
    GeoData *gd = (GeoData *)(getDataAtInd(data, dim-1)->data);
    NcValueChain *chain = createGeoChain(gd->x, gd->y, gd->z);
    NcNodeStack *stack = trailProperPath(nc, root, chain, dim);

    NcNode *child = NULL;
    NcNode *curr;
    NcNode *content;
    int update;
    while (!stackEmpty(stack)){
        curr = pop(stack);
        update = 0;
        
        if (curr->numChildren == 1){
            curr->content = child->content;
            setShared(curr->linkShared, 0, 1);
        } else if (curr->content.node == NULL){ //should cover content.timeseries being null too
            if (dim == (nc->numSpatialDim + nc->numCategoricalDim)){
                curr->content.timeseries = newTimeseries();
            } else {
                if (nc->dimensions[dim] == GEO){
                    curr->content.node = newGeoNode(0,0,0);
                } else {
                    curr->content.node = newCatNode(0);
                }
            }
            setShared(curr->linkShared, 0, 0);
            update = 1;
        } else if (checkShared(curr->linkShared, 0) && !nodeInList(curr, updatedList, *numUpdated)) {
            if (dim < (nc->numSpatialDim + nc->numCategoricalDim)){
                curr->content.node = shallowCopyNode(curr->content.node);
            } else {
                curr->content.timeseries = deepCopyTimeseries(curr->content.timeseries);
            }
            setShared(curr->linkShared, 0, 0);
            update = 1;
        } else if (!checkShared(curr->linkShared, 0)) {
            update = 1;
        }
        
        if (update){
            if (dim == (nc->numSpatialDim + nc->numCategoricalDim)){
                insertData(curr, dim, (nc->numSpatialDim + nc->numCategoricalDim), getDataAtInd(data, dim));
            } else if (dim < nc->numSpatialDim){
                addGeo(nc, curr->content.node, data, dim+1, updatedList, numUpdated);
            } else {
                addCat(nc, curr->content.node, data, dim+1, updatedList, numUpdated);
            }
            (*numUpdated)++;
            updatedList = realloc(updatedList, sizeof(NcNode *) * (*numUpdated));
            updatedList[(*numUpdated)-1] = curr->content.node;
        }
        child = curr;
    }

    //cleanup - free things here; may be better to hold some preallocated initial things globally
    freeChain(chain);
    freeStack(stack);
}

void addCat(Nanocube *nc, NcNode *root, NcData *data, int dim, NcNode **updatedList, size_t *numUpdated){
    /*
    As there can only be one level of categorical data, we've optimized to take advantage of this
    */
    CatData *cd = (CatData *)(getDataAtInd(data, dim-1)->data);
    NcValueChain *chain = createCatChain(cd->category);
    NcNode *curr = root;
    int update;

    NcNode *nodes[2];
    nodes[1] = root;
    size_t childInd = getMatchingChildInd(root, chain, 0, CAT);
    if (childInd == ((size_t)-1)){
        nodes[0] = newProperCatChild(root, chain, 0); //replace the stuff here with childless cat stuff
    } else if (checkShared(curr->linkShared, childInd)){
        nodes[0] = replaceChild(root, childInd, 1);
    } else {
        nodes[0] = root->children[childInd];
    }

    int i;
    for (i = 0; i < 2; i++){
        curr = nodes[i];
        update = 0;
        
        if (i == 1 && curr->numChildren == 1){
            curr->content = nodes[0]->content;
            setShared(curr->linkShared, 0, 1);
        } else if (curr->content.node == NULL){ //should cover content.timeseries too
            if (dim == (nc->numSpatialDim + nc->numCategoricalDim)){
                curr->content.timeseries = newTimeseries();
            } else {
                curr->content.node = newCatNode(0);
            }
            setShared(curr->linkShared, 0, 0);
            update = 1;
        } else if (checkShared(curr->linkShared, 0) && !nodeInList(curr, updatedList, *numUpdated)){
            if (dim < (nc->numSpatialDim + nc->numCategoricalDim)){
                curr->content.node = shallowCopyNode(curr->content.node);
            } else {
                curr->content.timeseries = deepCopyTimeseries(curr->content.timeseries);
            }
            setShared(curr->linkShared, 0, 0);
            update = 1;
        } else if (!checkShared(curr->linkShared, 0)) {
            update = 1;
        }

        if (update){
            if (dim == (nc->numSpatialDim + nc->numCategoricalDim)){
                insertData(curr, dim, (nc->numSpatialDim + nc->numCategoricalDim), getDataAtInd(data, dim));
            } else {
                addCat(nc, curr->content.node, data, dim+1, updatedList, numUpdated);
            }
            (*numUpdated)++;
            updatedList = realloc(updatedList, sizeof(NcNode *) * (*numUpdated));
            updatedList[(*numUpdated)-1] = curr->content.node;
        }
    }

    //cleanup - free things here; may be better to hold some preallocated initial things globally
    freeChain(chain);
}

NcNodeStack *trailProperPath(Nanocube *nc, NcNode *root, NcValueChain *values, int dim){
    NcNodeStack *stack = newNcNodeStack();
    push(stack, root);
    NcNode *curr = root;
    NcNode *child = NULL;
    size_t childInd = -1;
    size_t i;
    for (i = 0; i < values->num; i++){
        if (nc->dimensions[dim-1] == GEO){
            childInd = getMatchingChildInd(curr, values, i, GEO);
        } else { //categorical
            childInd = getMatchingChildInd(curr, values, i, CAT);
        }

        if (childInd == ((size_t)-1)){
            child = newProperChild(curr, values, i);
        } else if (checkShared(curr->linkShared, childInd)){
            child = replaceChild(curr, childInd, 0);
        } else {
            child = curr->children[childInd];
        }
        push(stack, child);
        curr = child;
    }
    return stack;
}

void printNanocube(Nanocube *self){
    printf("Num spatial dimensions: %d\n", self->numSpatialDim);//size_t is not a %d
    printf("Num categorical dimensions: %d\n", self->numCategoricalDim);//size_t is not a %d
    printMemUsage(self);
    NcNode *curr = self->root;
    printNode(curr, 0, 0, 0, self, 0);
}

void printNode(NcNode *self, int padding, int isShared, int isContent, Nanocube *nc, int dim){
    /*
        Should rewrite so that if a link isn't proper it only prints the address
    */

    if (self == NULL){
        return;
    }

    int i;
    for (i = 0; i < padding; i++){
        printf(" ");
    }

    unsigned char z;
    unsigned long long x, y, category;
    char shared = isShared ? 'S' : 'P';
    char content = isContent ? 'C' : 'N';
    if (dim < nc->numSpatialDim){
        z = decodeGeoKey(self->key, &x, &y);
        printf("%c : GEO : %lu,%lu,%c : %c : %p\n", content, x, y, z, shared, self);
    } else if (dim < nc->numCategoricalDim + nc->numSpatialDim) {
        category = decodeCatKey(self->key);
        printf("%c : CAT : %lu : %c : %p\n", content, category, shared, self);
    } else { //TIME
        printf("ought not be here\n");
    }

    if (dim < nc->numSpatialDim || decodeCatKey(self->key) == 0){
        for (i = 0; i < self->numChildren; i++){
            printNode(self->children[i], padding+1, checkShared(self->linkShared, i+1), 0, nc, dim);
        }
    }

    if (checkShared(self->linkShared, 0)){
        printf("S - ");
    } else {
        printf("P - ");
    }

    if (dim != (nc->numSpatialDim + nc->numCategoricalDim) - 1){
        printNode(self->content.node, padding+1, checkShared(self->linkShared, 0), 1, nc, dim+1);
    } else {
        printTimeseries(self->content.timeseries);
    }
}

void printStack(NcNodeStack *self, Nanocube *nc, int dim){
    printf("Stack:\n");
    NcNodeStack *curr;
    curr = self;
    while(curr != NULL){
        printNode(curr->node, 0, 0, 0, nc, dim);
        curr = curr->next;
    }
}

void printMemUsage(Nanocube *self){
    char suffix;
    int mem;

    if (self->memCount >= 1073741824){
        suffix = 'G';
        mem = self->memCount / 1073741824;
    } else if (self->memCount >= 1048576){
        suffix = 'M';
        mem = self->memCount / 1048576;
    } else if (self->memCount >= 1024){
        suffix = 'K';
        mem = self->memCount / 1024;
    } else {
        suffix = ' ';
        mem = self->memCount;
    }

    printf("Count: %llu, memory: %d%c\n", self->dataCount, mem, suffix);
}

