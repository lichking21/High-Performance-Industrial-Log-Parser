#include "generator.h"
#include <stdlib.h>
#include <stdio.h>

Data NewData()
{
    Data data = GenerateRandomData();

    return data;
}

Data GenerateRandomData()
{
    long min_date = 1072915200, max_date = 1767225600; // from 1.1.2004 to 1.1.2026
    long min_id = 1, max_id = 1000000;
    int min_type = 0, max_type = 2;
    double min_val = 1.0, max_val = 10000.0;
    int min_status = 0, max_status = 2;

    long timestamp = get_rand_timestamp(min_date, max_date);
    long sensor_id = get_rand_sensor_id(min_id, max_id);
    Type type = get_rand_type(min_type, max_type);
    double value = get_rand_val(min_val, max_val);
    Status status = get_rand_status(min_status, max_status);

    Data rand_data = {
        .Timestamp = timestamp,
        .SensorId = sensor_id,
        .Type = type,
        .Value = value,
        .Status = status,
    };

    return rand_data;
}

void print_data(Data data)
{
    char* type = NULL;
    char* status = NULL;

    if (data.Type == TEMPERATURE)       type = "TEMPERATURE";
    else if (data.Type == PRESSURE)     type = "PRESSURE";
    else if (data.Type == VIBRATION)    type = "VIBRATION";

    if (data.Status == OK)          status = "OK";
    else if (data.Status == WARN)   status = "WARN";
    else if (data.Status == ERR)    status = "ERR";

    printf("Date:       %ld\n", data.Timestamp);
    printf("Sensor ID:  %ld\n", data.SensorId);
    printf("Type:       %s\n", type);
    printf("Value:      %f\n", data.Value);
    printf("Status:     %s\n", status);
}

long get_rand_timestamp(long min_date, long max_date)
{
    return (rand() % (max_date - min_date + 1)) + min_date;
}
long get_rand_sensor_id(long min_id, long max_id)
{
    return (rand() % (max_id - min_id + 1)) + min_id;
}
Type get_rand_type(int min, int max)
{
    return (rand() % (max - min + 1)) + min;
}
double get_rand_val(double min, double max)
{
    double scale = (double)rand() / ((double)RAND_MAX + 1.0);
    return min + scale * (max - min);
}
Status get_rand_status(int min, int max)
{
    return (rand() % (max - min + 1)) + min;
}
