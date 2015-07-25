#include "timeseries.h"
#include <stdlib.h>

Timeseries *newTimeseries(){
    Timeseries *result = malloc(sizeof(Timeseries));
    result->bins = NULL;
    result->numBins = 0;
    return result;
}

void addToTimeseries(Timeseries *self, int time, unsigned long long count){
    if (time > self->numBins-1){
        self->bins = realloc(self->bins, sizeof(unsigned long long) * (self->numBins + 1));
    }
    self->bins[time] = count;
}
