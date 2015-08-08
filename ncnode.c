#include "ncnode.h"
#include <stdlib.h>

NcNode *newNcNode(NcDataType type){
    NcNode *result = malloc(sizeof(NcNode));
    
    result->children = NULL;
    result->numChildren = 0;
    result->isShared = NULL;
    result->content.node = NULL;
    result->sharedContent = 0;

    return result;
}

NcNode *newGeoNode(int x, int y, int z){
    NcNode *result = newNcNode(GEO);
    result->key.geo = newGeoKey(x, y, z);
    return result;
}

NcNode *newCatNode(int category){
    NcNode *result = newNcNode(CAT);
    result->key.cat = newCatKey(category);
    return result;
}

NcNodeStack *newNcNodeStack(){
    NcNodeStack *result = malloc(sizeof(NcNodeStack));
    result->node = NULL;
    result->next = NULL;
    return result;
}

void push(NcNodeStack *self, NcNode *node){
    NcNodeStack *copy = malloc(sizeof(NcNodeStack));
    copy->node = self->node;
    copy->next = self->next;
    self->node = node;
    self->next = copy;
}

NcNode *pop(NcNodeStack *self){
    NcNode *result = self->node;
    NcNodeStack *toFree = self->next;
    self->node = self->next->node;
    self->next = self->next->next;
    free(toFree);
    return result;
}

int stackEmpty(NcNodeStack *self){
    if (self->node == NULL && self->next == NULL){
        return 1;
    } else {
        return 0;
    }
}

int getMatchingChildInd(NcNode *self, NcValueChain *values, int index, NcDataType type){
    int result = -1;
    
    NcNode *child;
    GeoData gd;
    CatData cd;
    int i;
    unsigned long long x, y, cat;
    unsigned char z;
    for (i = 0; i < self->numChildren; i++){
        child = self->children[i];
        if (type == GEO){
            z = decodeGeoKey(child->key.geo, &x, &y);
            gd = ((GeoData *)values->data)[index];
            if (x == gd.x && y == gd.y && z == gd.z){
                result = i;
                break;
            }
        } else { //categorical
            cat = decodeCatKey(child->key.cat);
            cd = ((CatData *)values->data)[index];
            if (cat == cd.category){
                result = i;
                break;
            }
        }
    }

    return result;
}

NcNode *newProperChild(NcNode *self, NcValueChain *values, int index){
    NcNode *result;

    GeoData gd;
    CatData cd;
    if (values->type == GEO){
        gd = ((GeoData *)values->data)[index];
        result = newGeoNode(gd.x, gd.y, gd.z);
    } else {
        cd = ((CatData *)values->data)[index];
        result = newCatNode(cd.category);
    }

    self->numChildren++;
    self->isShared = realloc(self->isShared, sizeof(int) * self->numChildren);
    self->isShared[self->numChildren-1] = 0;
    self->children = realloc(self->children, sizeof(NcNode *) * self->numChildren);
    self->children[self->numChildren-1] = result;
    return result;
}

NcNode *replaceChild(NcNode *self, int index){
    self->children[index] = shallowCopyNode(self->children[index]);

}

NcNode *shallowCopyNode(NcNode *self){
    NcNode *copy = malloc(sizeof(NcNode));
    copy->key = self->key;

    if (self->content.node != NULL){
        copy->content.node = self->content.node;
        copy->sharedContent = 1;
    }

    copy->children = self->children;
    copy->numChildren = self->numChildren;
    copy->isShared = malloc(sizeof(self->numChildren));
    int i;
    for (i = 0; i < self->numChildren; i++){
        copy->isShared[i] = 1;
    }
    return copy;
}

int nodeInList(NcNode *self, NcNode **list, size_t size){
    //would it be smarted to compare pointer values? - YES
    int result = 0;
    int i;
    for (i = 0; i < size; i++){
        if (list[i] == self){
            result = 1;
            break;
        }
    }
    return result;
}

void insertData(NcNode *self, int dim, int timeDim, NcData *data){
    TimeData *td;
    if (dim != timeDim){
        insertData(self->content.node, dim+1, timeDim, data);
    } else {
        td = (TimeData *)data->data;
        addToTimeseries(self->content.timeseries, td->time, td->count);
    }
}

