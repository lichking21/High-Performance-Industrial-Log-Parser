#ifndef STATS_H
#define STATS_H

#include <stdio.h>
#include "data.h"

typedef struct
{
    size_t count;
    double sum;
    double min;
    double max;
} ValueStats;

typedef struct
{
    size_t total_records;
    size_t corrupted_records;

    ValueStats temperature;
    ValueStats pressure;
    ValueStats vibration;

    size_t status_ok;
    size_t status_warn;
    size_t status_err;

    double processing_time;
    double throughput;

} Stats;

void stats_init(Stats* stats);
int stats_upd(Stats* stats, Data* data);
void stats_print(Stats stats);

#endif
