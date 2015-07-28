#include "nanocube.h"
#include "ncquery.h"
#include "ncdata.h"
#include <stdio.h>

int main(int argc, const char *argv[]){
    printf("Start\n");
    Nanocube *nc = newNanocube(1, 1);
    printf("Made a nanocube!\n");
    //addToNanocube(nc, 1, 5, 1, 2);
    addToNanocube(nc, 2, 1, 1, 5, 1);
    printf("Added to the nanocube!\n");
    printNanocube(nc);


    //query testing
    NcQuery *q = newQuery(nc);
    addCatConstraint(q, 1, (CatData *)newCatData(1)->data, 0);
    query(nc, q);    
}
