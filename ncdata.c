#include "ncdata.h"
#include <stdlib.h>
#include <stdio.h>

NcValueChain *createGeoChain(unsigned long long x, unsigned long long y, unsigned char depth){
    NcValueChain *result = malloc(sizeof(NcValueChain));
    result->type = GEO;
    result->num = depth;
    GeoData *data = malloc(sizeof(GeoData) * depth);
    int i;
    for (i = 1; i <= depth; i++){
        data[i-1].x = x / (1 << (depth - i));
        data[i-1].y = y / (1 << (depth - i));
        data[i-1].z = i;
    }
    result->data = data;
    return result;
}

NcValueChain *createCatChain(unsigned long long category){
    NcValueChain *result = malloc(sizeof(NcValueChain));
    result->type = CAT;
    result->num = 1;
    CatData *data = malloc(sizeof(CatData));
    data[0].category = category;
    result->data = data;
    return result;
}

void freeChain(NcValueChain *self){
    free(self->data);
    free(self);
}

void printChain(NcValueChain *self){
    int i;
    GeoData gd;
    CatData cd;

    for (i = 0; i < self->num; i++){
        if (self->type == GEO){
            gd = ((GeoData *)self->data)[i];
            printf("%d,%d,%d", gd.x, gd.y, gd.z);
        } else if (self->type == CAT){
            cd = ((CatData *)self->data)[i];
            printf("%d", cd.category);
        } else {
            printf("Chain with invalid type!");
            break;
        }

        if (i != ((int)self->num) - 1){
            printf(" -> ");
        }
    }
    printf("\n");
}

NcData *newGeoData(unsigned long long x, unsigned long long y, unsigned char z){
    GeoData *gd = malloc(sizeof(GeoData));
    gd->x = x;
    gd->y = y;
    gd->z = z;
    NcData *result = malloc(sizeof(NcData));
    result->next = NULL;
    result->data = gd;
    return result;
}

NcData *newCatData(unsigned long long category){
    CatData *cd = malloc(sizeof(CatData));
    cd->category = category;
    NcData *result = malloc(sizeof(NcData));
    result->next = NULL;
    result->data = cd;
    return result;
}
NcData *newTimeData(size_t time, unsigned long long count){
    TimeData *td = malloc(sizeof(TimeData));
    td->time = time;
    td->count = count;
    NcData *result = malloc(sizeof(NcData));
    result->next = NULL;
    result->data = td;
    return result;
}

NcData *getDataAtInd(NcData *self, size_t index){
    size_t i;
    NcData *curr = self;
    for (i = 0; i < index; i++){
        curr = curr->next;
    }
    return curr;
}
