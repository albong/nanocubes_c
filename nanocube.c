#include "nanocube.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

//static void add(Nanocube *nc, NcNode *root, int x, int y, int cat, int time, int dim, NcNode **updatedList, size_t *numUpdated);
static void add(Nanocube *nc, NcNode *root, NcData *data, int dim, NcNode **updatedList, size_t *numUpdated);
static void printNode(NcNode *self, int padding, int isShared, int isContent, Nanocube *nc, int dim);

Nanocube *newNanocube(size_t numSpatialDim, size_t numCategoricalDim){
    Nanocube *result = malloc(sizeof(Nanocube));
    int i;
    
    result->numSpatialDim = numSpatialDim;
    result->numCategoricalDim = numCategoricalDim;

    if (numSpatialDim > 0){
        result->root = newGeoNode(0,0,0);
    } else {
        result->root = newCatNode(-1);
    }

    result->numDim = numSpatialDim + numCategoricalDim;
    result->dimensions = malloc(sizeof(NcDataType) * (numSpatialDim + numCategoricalDim));
    for (i = 0; i < result->numDim; i++){
        if (i < numSpatialDim){
            result->dimensions[i] = GEO;
        } else {
            result->dimensions[i] = CAT;
        }
    }
    return result; 
}

void addToNanocube(Nanocube *nc, int time, unsigned long long count, ...){
    va_list input;
    int i;
    NcData *data;
    NcData *curr;

    va_start(input, count);
    int x = va_arg(input, int);
    int y = va_arg(input, int);
    data = newGeoData(x, y, MAX_GEO_DEPTH);
    curr = data;
    for (i = 1; i < (nc->numSpatialDim + nc->numCategoricalDim); i++){//we always must have at least one spatial dim
        if (i < nc->numSpatialDim){
            x = va_arg(input, int);
            y = va_arg(input, int);
            curr->next = newGeoData(x, y, MAX_GEO_DEPTH);
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
    add(nc, nc->root, data, 1, NULL, &numUpdated);//fix the dim shenangians to be 0 indexed?
}

void add(Nanocube *nc, NcNode *root, NcData *data, int dim, NcNode **updatedList, size_t *numUpdated){
    NcValueChain *chain;
    if (nc->dimensions[dim-1] == GEO){
        chain = createChain(data, GEO, dim-1);
    } else {
        chain = createChain(data, CAT, dim-1);
    }
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
            curr->sharedContent = 1;
        } else if (curr->content.node == NULL){ //should cover content.timeseries being null too
            if (dim == nc->numDim){
//                curr->content = newTimeNode();
                curr->content.timeseries = newTimeseries();
            } else {
                if (nc->dimensions[dim] == GEO){
                    curr->content.node = newGeoNode(0,0,0);
                } else {
                    curr->content.node = newCatNode(-1);
                }
            }
            curr->sharedContent = 0;
            update = 1;
        } else if (curr->sharedContent && !nodeInList(curr, updatedList, *numUpdated)) {
            if (dim < nc->numDim){
                curr->content.node = shallowCopyNode(curr->content.node);
            } else {
                curr->content.timeseries = deepCopyTimeseries(curr->content.timeseries);
            }
            curr->sharedContent = 0;
            update = 1;
        } else if (!curr->sharedContent) {
            update = 1;
        }
        
        if (update){
            if (dim == nc->numDim){
                insertData(curr, dim, nc->numDim, getDataAtInd(data, dim));
            } else {
                add(nc, curr->content.node, data, dim+1, updatedList, numUpdated);
            }
            (*numUpdated)++;
            updatedList = realloc(updatedList, sizeof(NcNode *) * (*numUpdated));
            updatedList[(*numUpdated)-1] = curr->content.node;
        }
        child = curr;
    }

    //cleanup - free things here; may be better to hold some preallocated initial things globally
}

NcNodeStack *trailProperPath(Nanocube *nc, NcNode *root, NcValueChain *values, int dim){
    NcNodeStack *stack = newNcNodeStack();
    push(stack, root);
    NcNode *curr = root;
    NcNode *child = NULL;
    int childInd = -1;

    int i;
    for (i = 0; i < values->num; i++){
        if (nc->dimensions[dim-1] == GEO){
            childInd = getMatchingChildInd(curr, values, i, GEO);
        } else { //categorical
            childInd = getMatchingChildInd(curr, values, i, CAT);
        }

        if (childInd == -1){
            child = newProperChild(curr, values, i);
        } else if (curr->isShared[childInd]){
            child = replaceChild(curr, childInd);
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

    GeoNode *gn;
    CatNode *cn;
    TimeNode *tn;
    char shared = isShared ? 'S' : 'P';
    char content = isContent ? 'C' : 'N';
    if (dim < nc->numSpatialDim){
        gn = self->node.geo;
        printf("%c : GEO : %d,%d,%d : %c : %p\n", content, gn->x, gn->y, gn->z, shared, self);
    } else if (dim < nc->numCategoricalDim + nc->numSpatialDim) {
        cn = self->node.cat;
        printf("%c : CAT : %d : %c : %p\n", content, cn->category, shared, self);
    } else { //TIME
//        tn = (TimeNode *)self->node;
//        printf("%c : TIME :: %c : %p\n", content, shared, self);
        printf("ought not be here\n");
    }

    for (i = 0; i < self->numChildren; i++){
        printNode(self->children[i], padding+1, self->isShared[i], 0, nc, dim);
    }

    if (dim != nc->numDim - 1){
        printNode(self->content.node, padding+1, self->sharedContent, 1, nc, dim+1);
    } else {
        printTimeseries(self->content.timeseries);
    }
}
