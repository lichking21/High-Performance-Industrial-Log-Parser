#include "stats.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

void stats_init(Stats* stats)
{
    stats->total_records = 0;
    stats->corrupted_records = 0;

    stats->temperature.count = 0;
    stats->temperature.max = DBL_MAX;
    stats->temperature.min = DBL_MIN;
    stats->temperature.sum = 0.0;

    stats->pressure.count = 0;
    stats->pressure.max = DBL_MAX;
    stats->pressure.min = DBL_MIN;
    stats->pressure.sum = 0.0;

    stats->vibration.count = 0;
    stats->vibration.max = DBL_MAX;
    stats->vibration.min = DBL_MIN;
    stats->vibration.sum = 0.0;

    stats->status_ok = 0;
    stats->status_warn = 0;
    stats->status_err = 0;

    stats->processing_time = 0.0;
    stats->throughput = 0.0;
}
int stats_upd(Stats* stats, Data* data);
void stats_print(Stats stats);
