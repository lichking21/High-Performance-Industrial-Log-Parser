#include "stats.h"
#include "data.h"
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

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

void upd_types(Stats* stats, const Data* data)
{
    switch(data->Type)
    {
        case TEMPERATURE:
            stats->temperature.count += 1;
            stats->temperature.max = (data->Value > stats->temperature.max) ? data->Value : stats->temperature.max;
            stats->temperature.min = (data->Value < stats->temperature.min) ? data->Value : stats->temperature.min;
            stats->temperature.sum += data->Value;
            break;
        case PRESSURE:
            stats->pressure.count += 1;
            stats->pressure.max = (data->Value > stats->pressure.max) ? data->Value : stats->pressure.max;
            stats->pressure.min = (data->Value < stats->pressure.min) ? data->Value : stats->pressure.min;
            stats->pressure.sum += data->Value;
            break;
        case VIBRATION:
            stats->vibration.count += 1;
            stats->vibration.max = (data->Value> stats->vibration.max) ? data->Value : stats->vibration.max;
            stats->vibration.min = (data->Value < stats->vibration.min) ? data->Value : stats->vibration.min;
            stats->vibration.sum += data->Value;
            break;
    }
}

void upd_statuses(Stats* stats, const Data* data)
{
    switch(data->Status)
    {
        case OK:
            stats->status_ok += 1;
            break;
        case WARN:
            stats->status_warn += 1;
            break;
        case ERR:
            stats->status_err += 1;
            break;
    }
}

int stats_upd(Stats* stats, Data* data, size_t records, size_t corrupted, double time, double throughput)
{
    if (stats == NULL || data == NULL)
        return -1;

    stats->total_records = records;
    stats->corrupted_records = corrupted;

    upd_types(stats, data);

    upd_statuses(stats, data);

    stats->processing_time = time;
    stats->throughput = throughput;

    return 0;
}

void stats_print(Stats stats)
{
    printf("Records:            %ld\n", stats.total_records);
    printf("Corrupted records:  %ld\n", stats.corrupted_records);

    printf("Temperature:\n");
    printf("  Count:    %ld\n", stats.temperature.count);
    printf("  MAX:      %.2f\n", stats.temperature.max);
    printf("  MIN:      %.2f\n", stats.temperature.min);
    printf("  SUM:      %.2f\n", stats.temperature.sum);
    printf("  AVG:      %.2f\n", stats.temperature.sum / stats.temperature.count);

    printf("Pressure:\n");
    printf("  Count:    %ld\n", stats.pressure.count);
    printf("  MAX:      %.2f\n", stats.pressure.max);
    printf("  MIN:      %.2f\n", stats.pressure.min);
    printf("  SUM:      %.2f\n", stats.pressure.sum);
    printf("  AVG:      %.2f\n", stats.pressure.sum / stats.pressure.count);

    printf("Vibration:\n");
    printf("  Count:    %ld\n", stats.vibration.count);
    printf("  MAX:      %.2f\n", stats.vibration.max);
    printf("  MIN:      %.2f\n", stats.vibration.min);
    printf("  SUM:      %.2f\n", stats.vibration.sum);
    printf("  AVG:      %.2f\n", stats.vibration.sum / stats.vibration.count);

    printf("Status:\n");
    printf("  OK:       %ld\n", stats.status_ok);
    printf("  WARN:     %ld\n", stats.status_warn);
    printf("  ERR:      %ld\n", stats.status_err);

    printf("Processing time:    %.2f\n", stats.processing_time);
    printf("Throughput:         %.2f MB/s\n", stats.throughput);
}
