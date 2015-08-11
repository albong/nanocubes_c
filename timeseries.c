#include "timeseries.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Timeseries *newTimeseries(){
    Timeseries *result = malloc(sizeof(Timeseries));
    addMemToCount(sizeof(unsigned long long) * 3);
    result->bins = NULL;
    result->numBins = 0;
    result->startBin = -1;
    return result;
}

void addToTimeseries(Timeseries *self, size_t time, unsigned long long count){
    size_t i;
    unsigned long long *newBins;

    if (self->numBins == 0){
        self->bins = malloc(sizeof(unsigned long long));
        self->startBin = time;
        self->numBins = 1;
    } else if (time < self->startBin){
        newBins = calloc(self->numBins + self->startBin - time, sizeof(unsigned long long));
        memcpy(newBins + (self->startBin - time), self->bins, sizeof(unsigned long long) * self->numBins);
        free(self->bins);
        self->bins = newBins;
        self->numBins = self->numBins + (self->startBin - time);
        self->startBin = time;

    } else if (time >= (self->startBin + self->numBins)){
        self->bins = realloc(self->bins, sizeof(unsigned long long) * (self->numBins + (time - (self->startBin + self->numBins) + 1)));
        for (i = (self->startBin + self->numBins); i <= time; i++){
            self->bins[i - self->startBin] = 0;
        }
        self->numBins = self->numBins + (time - (self->startBin + self->numBins) + 1);
    }

    self->bins[(time - self->startBin)] += count;
}

unsigned long long getCountAtTime(Timeseries *self, size_t time){
    unsigned long long result = 0;
    if (self->startBin <= time && time < (self->startBin + self->numBins)){
        result = self->bins[time - self->startBin];
    }
    return result;
}

void printTimeseries(Timeseries *self){
    if (self == NULL){
        printf("Null timeseries\n");
        return;
    }

    int i;
    printf("startBin: %d; numBins: %d; ", self->startBin, self->numBins);
    for (i = self->startBin; i < (self->startBin + self->numBins); i++){
        printf("%d:%llu ", i, self->bins[i - self->startBin]);
    }
    printf("\n");
}

Timeseries *deepCopyTimeseries(Timeseries *self){
    Timeseries *copy = malloc(sizeof(Timeseries));
    addMemToCount(sizeof(unsigned long long) * 3);

    copy->startBin = self->startBin;
    copy->numBins = self->numBins;
    copy->bins = malloc(sizeof(unsigned long long) * copy->numBins);
    addMemToCount(sizeof(unsigned long long) * copy->numBins);
    memcpy(copy->bins, self->bins, sizeof(unsigned long long) * self->numBins);

    return copy;
}
