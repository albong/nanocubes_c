#include "ncdata.h"
#include <stdlib.h>

NcValueChain *createGeoChain(int x, int y, size_t depth){
    NcValueChain *result = malloc(sizeof(NcValueChain));
    result->type = GEO;
    result->num = depth + 1;
    GeoData *data = malloc(sizeof(GeoData) * (depth + 1));
    int i;
    for (i = 0; i <= depth; i++){
        data[i].x = x / (1 << (depth - i));
        data[i].y = y / (1 << (depth - i));
        data[i].z = i;
    }
    result->data = data;
    return result;
}

NcValueChain *createCatChain(int category){
    return NULL;
}
