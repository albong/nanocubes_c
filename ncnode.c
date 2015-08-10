#include "ncnode.h"
#include <stdlib.h>
#include <limits.h>

NcNode *newNcNode(NcDataType type){
    NcNode *result = malloc(sizeof(NcNode));
    
    result->children = NULL;
    result->numChildren = 0;
    result->linkShared = malloc(sizeof(unsigned char) * 2);
    result->linkShared[0] = 1;
    result->content.node = NULL;
    result->linkShared[1] = 0; //set sharedContent to 0

    return result;
}

NcNode *newGeoNode(int x, int y, int z){
    NcNode *result = newNcNode(GEO);
    result->key = newGeoKey(x, y, z);
    return result;
}

NcNode *newCatNode(int category){
    NcNode *result = newNcNode(CAT);
    result->key = newCatKey(category);
    return result;
}

NcNode *newChildlessCatNode(unsigned long long category){
    CatNode *result = malloc(sizeof(CatNode));

    result->key = newCatKey(category);
    result->content.node = NULL;
    result->linkShared = malloc(sizeof(unsigned char) * 2);
    result->linkShared[0] = 1;
    result->linkShared[1] = 0; //set sharedContent to 0

    return (NcNode *)result;
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

void freeStack(NcNodeStack *self){
    NcNodeStack *curr = self;
    NcNodeStack *next;
    while(curr != NULL){
        next = curr->next;
        free(curr);
        curr = next;
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
            z = decodeGeoKey(child->key, &x, &y);
            gd = ((GeoData *)values->data)[index];
            if (x == gd.x && y == gd.y && z == gd.z){
                result = i;
                break;
            }
        } else { //categorical
            cat = decodeCatKey(child->key);
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
    setShared(self->linkShared, self->numChildren, 0);
    self->children = realloc(self->children, sizeof(NcNode *) * self->numChildren);
    self->children[self->numChildren-1] = result;
    return result;
}

NcNode *newProperCatChild(NcNode *self, NcValueChain *values, int index){
    CatData cd = ((CatData *)values->data)[index];
    NcNode *result = newChildlessCatNode(cd.category);

    self->numChildren++;
    setShared(self->linkShared, self->numChildren, 0);
    self->children = realloc(self->children, sizeof(NcNode *) * self->numChildren);
    self->children[self->numChildren-1] = result;

    return result;
}

NcNode *replaceChild(NcNode *self, int index, int childIsCat){
    if (childIsCat){
       self->children[index] = shallowCopyCatNode((CatNode *)self->children[index]);
    } else {
       self->children[index] = shallowCopyNode(self->children[index]);
    }
}

NcNode *shallowCopyNode(NcNode *self){
    NcNode *copy = malloc(sizeof(NcNode));
    copy->key = self->key;

    copy->linkShared = malloc(sizeof(unsigned char) * 2);
    copy->linkShared[0] = 1;
    copy->linkShared[1] = 0;
    if (self->content.node != NULL){
        copy->content.node = self->content.node;
        setShared(copy->linkShared, 0, 1);
    }

    copy->children = self->children;
    copy->numChildren = self->numChildren;
    int i;
   for (i = 0; i < self->numChildren; i++){
        setShared(copy->linkShared, i + 1, 1);
    }
    return copy;
}

NcNode *shallowCopyCatNode(CatNode *self){
    CatNode *copy = malloc(sizeof(CatNode));
    copy->key = self->key;

    copy->linkShared = malloc(sizeof(unsigned char) * 2);
    copy->linkShared[0] = 1;
    copy->linkShared[1] = 0;
    if (self->content.node != NULL){
        copy->content.node = self->content.node;
        setShared(copy->linkShared, 0, 1);
    }

    return (NcNode *)copy;
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

int checkShared(unsigned char *self, size_t index){
    unsigned char byte = self[1 + (index / CHAR_BIT)];
    return (byte >> (index % CHAR_BIT)) & 1;
}

unsigned char *setShared(unsigned char *self, size_t index, int shared){
    int numBytes = self[0];
    if (((index / CHAR_BIT) + 1) > numBytes){
        self = realloc(self, sizeof(unsigned char) * (2 + (index / CHAR_BIT)));
        self[0] = (index / CHAR_BIT) + 1;
    }
    
    if (shared){//set
        self[1 + (index / CHAR_BIT)] |= (1 << (index % CHAR_BIT));
    } else {//clear
        self[1 + (index / CHAR_BIT)] &= ~(1 << (index % CHAR_BIT));
    }
}

