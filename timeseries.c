#include "timeseries.h"
#include <stdlib.h>
#include <stdio.h>

Timeseries *newTimeseries(){
    Timeseries *result = malloc(sizeof(Timeseries));
    result->bins = NULL;
    result->numBins = 0;
    result->startBin = -1;
    return result;
}

void addToTimeseries(Timeseries *self, size_t time, unsigned long long count){
    if (time < self->startBin || time >= (self->startBin + self->numBins)){
        self->bins = realloc(self->bins, sizeof(unsigned long long) * self->numBins);
        self->startBin = (time < self->startBin) ? time : self->startBin;
        self->numBins = (time >= (self->startBin + self->numBins)) ? (time - self->startBin + 1) : self->numBins;
    }

/*    if (time+1 > self->numBins){
        self->bins = realloc(self->bins, sizeof(unsigned long long) * (time+1));
        self->numBins = time + 1;
    }*/

    self->bins[time] = count;
}

unsigned long long getCountAtTime(Timeseries *self, size_t time){
    unsigned long long result = 0;
    if (time < self->numBins){
        result = self->bins[time];
    }
    return result;
}

void printTimeseries(Timeseries *self){
    int i;
    printf("startBin: %d; numBins: %d; ", self->startBin, self->numBins);
    for (i = self->startBin; i < (self->startBin + self->numBins); i++){
        printf("%d:%lu ", i, self->bins[i]);
    }
    printf("\n");
}

Timeseries *deepCopyTimeseries(Timeseries *self){
    Timeseries *copy = malloc(sizeof(Timeseries));

    copy->startBin = self->startBin;
    copy->numBins = self->numBins;
    copy->bins = malloc(sizeof(unsigned long long) * copy->numBins);
    size_t i;
    for (i = copy->startBin; i < (copy->startBin + copy->numBins); i++){
        copy->bins[i] = self->bins[i];
    }

    return copy;
}
