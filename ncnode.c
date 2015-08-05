#include "ncnode.h"
#include <stdlib.h>

//static GeoNode *makeGeoNode(int x, int y, int z);
//static CatNode *makeCatNode(int category);

NcNode *newNcNode(NcDataType type){
    NcNode *result = malloc(sizeof(NcNode));
    
    result->children = NULL;
    result->numChildren = 0;
    result->isShared = NULL;
    result->content.node = NULL;
    result->sharedContent = 0;

//    if (type == GEO){
        //seems we may need to ammend the signature to take a pointer to fill in
        //uses less memory to not hold a pointer, just to use the address
//        result->node.geo = makeGeoNode(0,0,0);
//    } else {
//        result->node.cat = makeCatNode(-1);
//    }

    return result;
}

NcNode *newGeoNode(int x, int y, int z){
    NcNode *result = newNcNode(GEO);
    newGeoKey(&result->key.geo, x, y, z);
    return result;
}

NcNode *newCatNode(int category){
    NcNode *result = newNcNode(CAT);
    newCatKey(&result->key.cat, category);
    return result;
}
/*
GeoNode *makeGeoNode(int x, int y, int z){
    GeoNode *result = malloc(sizeof(GeoNode));
    result->x = x;
    result->y = y;
    result->z = z;
    return result;
}

CatNode *makeCatNode(int category){
    CatNode *result = malloc(sizeof(CatNode));
    result->category = category;
    return result;
}
*/
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

NcNode *getMatchingChild(NcNode *self, NcValueChain *values, int index, NcDataType type){
    NcNode *result = NULL;
    
    NcNode *child;
    GeoData gd;
    CatData cd;
//    GeoNode *gn;
//    CatNode *cn;
    int i;
    unsigned long long x, y, cat;
    unsigned char z;
    for (i = 0; i < self->numChildren; i++){
        child = self->children[i];
//        if (child->type == GEO && values->type == GEO){
        if (type == GEO){
//            gn = child->node.geo;
            z = decodeGeoKey(child->key.geo, &x, &y);
            gd = ((GeoData *)values->data)[index];
//            if (gn->x == gd.x && gn->y == gd.y && gn->z == gd.z){
            if (x == gd.x && y == gd.y && z == gd.z){
                result = child;
                break;
            }
//        } else if (child->type = CAT && values->type == CAT) {
        } else { //categorical
//            cn = child->node.cat;
            cat = decodeCatKey(child->key.cat);
            cd = ((CatData *)values->data)[index];
            if (cat == cd.category){
//            if (cn->category == cd.category){
                result = child;
                break;
            }
        }
    }

    return result;
}

int getMatchingChildInd(NcNode *self, NcValueChain *values, int index, NcDataType type){
    int result = -1;
    
    NcNode *child;
    GeoData gd;
    CatData cd;
    GeoNode *gn;
    CatNode *cn;
    int i;
    unsigned long long x, y, cat;
    unsigned char z;
    for (i = 0; i < self->numChildren; i++){
        child = self->children[i];
//        if (child->type == GEO && values->type == GEO){
        if (type == GEO){
//            gn = child->node.geo;
            z = decodeGeoKey(child->key.geo, &x, &y);
            gd = ((GeoData *)values->data)[index];
            if (x == gd.x && y == gd.y && z == gd.z){
//            if (gn->x == gd.x && gn->y == gd.y && gn->z == gd.z){
                result = i;
                break;
            }
//        } else if (child->type = CAT && values->type == CAT) {
        } else { //categorical
//            cn = child->node.cat;
            cat = decodeCatKey(child->key.cat);
            cd = ((CatData *)values->data)[index];
            if (cat == cd.category){
//            if (cn->category == cd.category){
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
//    copy->type = self->type;
//    copy->node = self->node; //this may be bad, may want to deep copy this
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

