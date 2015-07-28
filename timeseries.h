#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <stddef.h>

typedef struct Timeseries {
    unsigned long long *bins; //array
    size_t numBins;
} Timeseries;

Timeseries *newTimeseries();
void addToTimeseries(Timeseries *self, int time, unsigned long long count);
void printTimeseries(Timeseries *self);

#endif
