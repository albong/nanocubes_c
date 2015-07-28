#include "timeseries.h"
#include <stdlib.h>
#include <stdio.h>

Timeseries *newTimeseries(){
    Timeseries *result = malloc(sizeof(Timeseries));
    result->bins = NULL;
    result->numBins = 0;
    return result;
}

void addToTimeseries(Timeseries *self, int time, unsigned long long count){
    if (time+1 > self->numBins){
        self->bins = realloc(self->bins, sizeof(unsigned long long) * (time+1));
        self->numBins = time + 1;
    }
    self->bins[time] = count;
}

unsigned long long getCountAtTime(Timeseries *self, int time){
    unsigned long long result = 0;
    if (time < self->numBins){
        result = self->bins[time];
    }
    return result;
}

void printTimeseries(Timeseries *self){
    int i;
    printf("numBins: %d; ", self->numBins);
    for (i = 0; i < self->numBins; i++){
        printf("%d:%lu ", i, self->bins[i]);
    }
    printf("\n");
}
