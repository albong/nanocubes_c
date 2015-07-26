#include "nanocube.h"

#include <stdlib.h>
#include <stdio.h>

static void add(Nanocube *nc, NcNode *root, int x, int y, int cat, int time, int dim, NcNode **updatedList, size_t *numUpdated);
static NcNodeStack *trailProperPath(Nanocube *nc, NcNode *root, NcValueChain *values);
static void printNode(NcNode *self, int padding, int isShared, int isContent);

Nanocube *newNanocube(size_t numSpatialDim, size_t numCategories){
    Nanocube *result = malloc(sizeof(Nanocube));
    int i;
    
    result->numSpatialDim = numSpatialDim;
    result->numCategories = numCategories;

    if (numSpatialDim > 0){
        result->root = newGeoNode(0,0,0);
    } else {
        result->root = newConNode(-1);
    }

    result->numDim = numSpatialDim + numCategories;
    result->dimensions = malloc(sizeof(NcDataType) * (numSpatialDim + numCategories));
    for (i = 0; i < result->numDim; i++){
        if (i < numSpatialDim){
            result->dimensions[i] = GEO;
        } else {
            result->dimensions[i] = CAT;
        }
    }
    return result; 
}

void addToNanocube(Nanocube *nc, int x, int y, int cat, int time){
    size_t numUpdated = 0;
    add(nc, nc->root, x, y, cat, time, 1, NULL, &numUpdated);//fix the dim shenangians to be 0 indexed
}

void add(Nanocube *nc, NcNode *root, int x, int y, int cat, int time, int dim, NcNode **updatedList, size_t *numUpdated){
    NcValueChain *chain;
    if (nc->dimensions[dim-1] == GEO){
        chain = createGeoChain(x, y, MAX_GEO_DEPTH);
        printChain(chain);
    } else {
        chain = createCatChain(cat);
    }
    NcNodeStack *stack = trailProperPath(nc, root, chain);

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
        } else if (curr->content == NULL){
            if (dim == nc->numDim){
                curr->content = newTimeNode();
            } else {
                if (nc->dimensions[dim-1] == GEO){
                    curr->content = newGeoNode(0,0,0);
                } else {
                    curr->content = newConNode(-1);
                }
            }
            curr->sharedContent = 0;
            update = 1;
        } else if (curr->sharedContent && !nodeInList(curr, updatedList, *numUpdated)) {
            curr->content = shallowCopyNode(curr->content);
            curr->sharedContent = 0;
            update = 1;
        } else if (!curr->sharedContent) {
            update = 1;
        }
        
        if (update){
            if (dim == nc->numDim){
                insert(curr, time, 1);
            } else {
                add(nc, curr->content, x, y, cat, time, dim+1, updatedList, numUpdated);
            }
            (*numUpdated)++;
            updatedList = realloc(updatedList, sizeof(NcNode *) * (*numUpdated));
            updatedList[(*numUpdated)-1] = curr->content;
        }
        child = curr;
    }

    //cleanup - free things here; may be better to hold some preallocated initial things globally
}

NcNodeStack *trailProperPath(Nanocube *nc, NcNode *root, NcValueChain *values){
    NcNodeStack *stack = newNcNodeStack();
    push(stack, root);
    NcNode *curr = root;
    NcNode *child = NULL;
    int childInd = -1;

    int i;
    for (i = 0; i < values->num; i++){
        childInd = getMatchingChildInd(curr, values, i);
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
    printf("Num categorical dimensions: %d\n", self->numCategories);//size_t is not a %d
    NcNode *curr = self->root;
    printNode(curr, 0, 0, 0);
}

void printNode(NcNode *self, int padding, int isShared, int isContent){
    if (self == NULL){
        return;
    }

    int i;
    for (i = 0; i < padding; i++){
        printf(" ");
    }

    GeoNode *gn;
    ConNode *cn;
    TimeNode *tn;
    char shared = isShared ? 'S' : 'P';
    char content = isContent ? 'C' : 'N';
    if (self->type == GEO){
        gn = (GeoNode *)self->node;
        printf("%c : GEO : %d,%d,%d : %c : %p\n", content, gn->x, gn->y, gn->z, shared, self);
    } else if (self->type == CAT) {
        cn = (ConNode *)self->node;
        printf("%c : CAT : %d : %c : %p\n", content, cn->category, shared, self);
    } else { //TIME
        tn = (TimeNode *)self->node;
        printf("%c : TIME :: %c : %p\n", content, shared, self);
    }

    for (i = 0; i < self->numChildren; i++){
        printNode(self->children[i], padding+1, self->isShared[i], 0);
    }
    printNode(self->content, padding+1, self->sharedContent, 1);
}
