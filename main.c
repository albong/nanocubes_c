#include "nanocube.h"
//#include "ncquery.h"
#include "ncdata.h"
#include "timeseries.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define TEST 1

void usage(){
    printf("Usage: nanocube -g number -c number -p port\n");
    printf("\t-g\tThe number of geographic dimensions\n");
    printf("\t-c\tThe number of categorical dimensions\n");
    printf("\t-p\tThe port to query the nanocube on\n");
}

void help(){
    printf("Reads from stdin, your data should be comma delimited.");
}

void readIn(Nanocube *nc){
    char DELIMITER = ',';
    char lineBuffer[BUFFER_SIZE];
    char dataBuffer[BUFFER_SIZE];
    int i, j = 0;
    int geoFound, catFound, timeFound, countFound;
    unsigned long long *x = malloc(sizeof(unsigned long long) * nc->numSpatialDim);
    unsigned long long *y = malloc(sizeof(unsigned long long) * nc->numSpatialDim);
    unsigned long long *cat = malloc(sizeof(unsigned long long) * nc->numCategoricalDim);
    unsigned long long time;
    unsigned long long count;

    while(1){
        geoFound = 0;
        catFound = 0;
        timeFound = 0;
        countFound = 0;

        if (fgets(lineBuffer, BUFFER_SIZE, stdin) == NULL){
            break;
        }

        for (i = 0; i < 1024; i++){
            if (lineBuffer[i] == ' '){
                continue;
            } else if (lineBuffer[i] == '\n'){
                if ((geoFound/2) == nc->numSpatialDim && catFound == nc->numCategoricalDim && timeFound == 1 && countFound == 0){
                    count = strtoull(dataBuffer, NULL, 10);
                    countFound++;
                    j = 0;
                }
                if ((geoFound/2) != nc->numSpatialDim || catFound != nc->numCategoricalDim || timeFound != 1 || countFound != 1 || j != 0){
                    //skip me
                    printf("FAILURE 2\n");
                } else {
                    //add me
                    printf("SUCCESS\n");
                    printf("%d, %d\n", x[0], y[0]);
                    addArraysToNanocube(nc, x, y, cat, time, count);
                }
                break;
            } else if (lineBuffer[i] == DELIMITER){
                dataBuffer[j] = '\0';
                printf("%d,%d,%d,%d\n", geoFound, catFound, timeFound, countFound);

                if (geoFound < (nc->numSpatialDim * 2)){
                    if (geoFound % 2 == 0){
                        x[geoFound/2] = strtoull(dataBuffer, NULL, 10);
                        printf("%s\n", dataBuffer);
                    } else {
                        y[geoFound/2] = strtoull(dataBuffer, NULL, 10);
                    }
                    geoFound++;
                } else if (catFound < nc->numCategoricalDim){
                    cat[catFound] = strtoull(dataBuffer, NULL, 10);
                    catFound++;
                } else if (timeFound < 1){
                    time = strtoull(dataBuffer, NULL, 10);
                    timeFound++;
                } else if (countFound < 1){
                    count = strtoull(dataBuffer, NULL, 10);
                    countFound++;
                } else {
                    //some flag to skippy skip
                    printf("FAILURE 1\n");
                }

                j = 0;
            } else {
                printf("%s - %c - %d\n", dataBuffer, lineBuffer[i], j);
                dataBuffer[j] = lineBuffer[i];
                j++;
            }
        }
    }
}

void test(){
    printf("Start\n");
    Nanocube *nc = newNanocube(1, 1);
    printf("Made a nanocube!\n");
    //addToNanocube(nc, 1, 5, 1, 2);
    //addToNanocube(nc, 2, 142123, 1, 5, 1);
    addToNanocube(nc, 10, 123, 1, 5, 1);
    printNanocube(nc);
    printf("\n\n");
    addToNanocube(nc, 2, 142123, 15, 5, 1);
    printNanocube(nc);
    printf("\n\n");
    addToNanocube(nc, 5, 789, 15, 5, 1);
    printf("Added to the nanocube!\n");
    printNanocube(nc);

    //query testing
/*    CatConstraint cat1;
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
*/
}

void test2(){
    Nanocube *nc = newNanocube(1, 1);
    addToNanocube(nc, 2, 3, 15, 5, 1);
//    printNanocube(nc);
//    printf("\n\n");
    addToNanocube(nc, 5, 1, 15, 5, 1);
//    printNanocube(nc);
}

void timeseriesTest(){
    Timeseries *ts = newTimeseries();
    addToTimeseries(ts, 5, 1);
    printTimeseries(ts);
    addToTimeseries(ts, 5, 1);
    printTimeseries(ts);
    addToTimeseries(ts, 3, 1);
    printTimeseries(ts);
    addToTimeseries(ts, 6, 1);
    addToTimeseries(ts, 2, 10);
    addToTimeseries(ts, 4, 1);
    addToTimeseries(ts, 5, 1);
    addToTimeseries(ts, 12, 1);
    addToTimeseries(ts, 0, 1);
    addToTimeseries(ts, 10, 1);
    printTimeseries(ts);
}

int main(int argc, char *argv[]){
    int opt;
    int numGeo = -1;
    int numCat = -1;
    
    while ((opt = getopt(argc, argv, "g:c:h")) != -1) {
        switch (opt){
            case 'g':
                numGeo = atoi(optarg);
                break;
            case 'c':
                numCat = atoi(optarg);
                break;
            case 'h':
                usage();
                help();
                return 0;
            case '?':
            default:
                usage();
                return 1;
                break;
        }
    }

    if (numGeo < -1 || numCat < -1){
        usage();
        return 1;
    } else if (numGeo < 0){
        printf("Need at least one geographic dimension\n");
        usage();
        return 1;
    }

    if (!TEST){
       Nanocube *nc = newNanocube(numGeo, numCat);
       readIn(nc);
       printNanocube(nc);
    } else {
        test();
//        test2();
//        timeseriesTest();
    }




    return 0;
}
