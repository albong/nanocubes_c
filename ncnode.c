#include "ncnode.h"
#include <stdlib.h>

static GeoNode *makeGeoNode(int x, int y, int z);
static ConNode *makeConNode(int category);
static TimeNode *makeTimeNode();

NcNode *newNcNode(NcDataType type){
    NcNode *result = malloc(sizeof(NcNode));
    
    result->type = type;
    result->node = NULL;
    result->children = NULL;
    result->numChildren = 0;
    result->isShared = NULL;
    result->content = NULL;
    result->sharedContent = 0;

    if (type == GEO){
        result->node = (void *)makeGeoNode(0,0,0);
    } else if (type == CAT){
        result->node = (void *)makeConNode(-1);
    } else if (type == TIME){
        result->node = (void *)makeTimeNode();
    }

    return result;
}

NcNode *newGeoNode(int x, int y, int z){
    NcNode *result = newNcNode(GEO);
    GeoNode *node = (GeoNode *)result->node;
    node->x = x;
    node->y = y;
    node->z = z;
    return result;
}

NcNode *newConNode(int category){
    NcNode *result = newNcNode(CAT);
    ConNode *node = (ConNode *)result->node;
    node->category = category;
    return result;
}

NcNode *newTimeNode(){
    NcNode *result = newNcNode(TIME);
    return result;
}

GeoNode *makeGeoNode(int x, int y, int z){
    GeoNode *result = malloc(sizeof(GeoNode));
    result->x = x;
    result->y = y;
    result->z = z;
    return result;
}

ConNode *makeConNode(int category){
    ConNode *result = malloc(sizeof(ConNode));
    result->category = category;
    return result;
}

TimeNode *makeTimeNode(){
    TimeNode *result = malloc(sizeof(TimeNode));
    result->timeseries = newTimeseries();
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

NcNode *getMatchingChild(NcNode *self, NcValueChain *values, int index){
    NcNode *result = NULL;
    
    NcNode *child;
    GeoData gd;
    CatData cd;
    GeoNode *gn;
    ConNode *cn;
    int i;
    for (i = 0; i < self->numChildren; i++){
        child = self->children[i];
        if (child->type == GEO && values->type == GEO){
            gn = (GeoNode *)child->node;
            gd = ((GeoData *)values->data)[index];
            if (gn->x == gd.x && gn->y == gd.y && gn->z == gd.z){
                result = child;
                break;
            }
        } else if (child->type = CAT && values->type == CAT) {
            cn = (ConNode *)child->node;
            cd = ((CatData *)values->data)[index];
            if (cn->category == cd.category){
                result = child;
                break;
            }
        }
    }

    return result;
}

int getMatchingChildInd(NcNode *self, NcValueChain *values, int index){
    int result = -1;
    
    NcNode *child;
    GeoData gd;
    CatData cd;
    GeoNode *gn;
    ConNode *cn;
    int i;
    for (i = 0; i < self->numChildren; i++){
        child = self->children[i];
        if (child->type == GEO && values->type == GEO){
            gn = (GeoNode *)child->node;
            gd = ((GeoData *)values->data)[index];
            if (gn->x == gd.x && gn->y == gd.y && gn->z == gd.z){
                result = i;
                break;
            }
        } else if (child->type = CAT && values->type == CAT) {
            cn = (ConNode *)child->node;
            cd = ((CatData *)values->data)[index];
            if (cn->category == cd.category){
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
        result = newConNode(cd.category);
    }

    self->numChildren++;
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
    copy->type = self->type;
    copy->node = self->node; //this may be bad, may want to deep copy this

    if (self->content != NULL){
        copy->content = self->content;
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

/*
    GeoNode *gn, sg;
    ConNode *cn, sc;
    //may need to add time
    int i;
    for (i = 0; i < size; i++){
        if (list[i]->type == GEO){
            gn = (GeoNode *)list[i]->node;
            sg = (GeoNode *)self->node;
            if (gn->x == sg->x ** gn->y == sg->y && gn->z == sg->z){
                result = 1;
                break;
            }
        } else if (list[i]->type == CAT){
            cn = (ConNode *)list[i]->node;
            sc = (ConNode *)self->node;
            if (cn->category == sc->category){
                result = 1;
                break;
            }
        }
    }
*/
    int i;
    for (i = 0; i < size; i++){
        if (list[i] == self){
            result = 1;
            break;
        }
    }
    return result;
}

void insert(NcNode *self, int time, unsigned long long count){
    if (self->type != TIME){
        insert(self->content, time, count);
        return;
    }
    TimeNode *tn = (TimeNode *)self->node;
    addToTimeseries(tn->timeseries, time, count);
}
