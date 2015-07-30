#include "nanocube.h"
#include "ncquery.h"
#include "ncdata.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char *argv[]){
    printf("Start\n");
    Nanocube *nc = newNanocube(1, 1);
    printf("Made a nanocube!\n");
    //addToNanocube(nc, 1, 5, 1, 2);
    addToNanocube(nc, 2, 142123, 1, 5, 1);
    printf("Added to the nanocube!\n");
    printNanocube(nc);


    //query testing
    CatConstraint cat1;
    cat1.num = 1;
    cat1.categories = malloc(sizeof(int) * cat1.num);
    cat1.categories[0] = 1;
    printf("Making query\n");
    NcQuery *q = newQuery(nc);
    printf("adding categorical constraint\n");
    addCatConstraint(q, 1, &cat1, 0);
    printf("running query\n");
    NcResult *qResult = query(nc, q);
    printf("survived query\n");
    printResult(qResult, 0);
}
