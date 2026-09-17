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

void upd_types(Stats* stats, const Data* data);
void upd_statuses(Stats* stats, const Data* data);
void upd_time(Stats* stats, double processing_time, double throughput);
int stats_upd(Stats* stats, Data* data, size_t records, size_t corrupted);

void stats_print(const Stats* stats);

#endif
