#ifndef TIMESERIES_H
#define TIMESERIES_H

#include <stddef.h>

typedef struct Timeseries {
    unsigned long long *bins; //array
    size_t startBin;
    size_t numBins;
    //[start, end)
} Timeseries;

Timeseries *newTimeseries();
void addToTimeseries(Timeseries *self, size_t time, unsigned long long count);
unsigned long long getCountAtTime(Timeseries *self, size_t time);
void printTimeseries(Timeseries *self);
Timeseries *deepCopyTimeseries(Timeseries *self);

#endif
