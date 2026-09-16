#include "generator.h"
#include "data.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <sys/stat.h>

Data NewData()
{
    Data data = GenerateRandomData();

    return data;
}

int generate_csv(const char* filename, size_t target_size)
{
    FILE* fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("(ERR) >> Failed to open .csv file\n");
        return -1;
    }

    fprintf(fp, "Date,SensorID,Type,Value,Status\n");

    size_t file_size = 0;

    while(file_size < target_size)
    {
        Data data = GenerateRandomData();

        int written = write_to_csv(data, fp);

        if (written < 0)
        {
            printf("(ERR) >> Failed to write\n");
            fclose(fp);
            return -1;
        }

        file_size += (size_t)written;
    }

    printf(">> %s file was successfully generated\n", filename);
    printf(">> File size: %ld bytes", ftell(fp));
    fclose(fp);

    return 0;
}
int write_to_csv(Data data, FILE* fp)
{
    const char* type = type_to_str(data.Type);
    const char* status = status_to_str(data.Status);

    int bytes = fprintf(
        fp,
        "%ld,%ld,%s,%f,%s\n",
        data.Timestamp, data.SensorId, type, data.Value, status
    );

    return bytes;
}

Data GenerateRandomData()
{
    long min_date = 1072915200, max_date = 1767225600; // from 1.1.2004 to 1.1.2026
    long min_id = 1, max_id = 1000000;
    int min_type = 0, max_type = 2;
    double min_val = 1.0, max_val = 10000.0;
    int min_status = 0, max_status = 2;

    Data rand_data = {
        .Timestamp  = get_rand_timestamp(min_date, max_date),
        .SensorId   = get_rand_sensor_id(min_id, max_id),
        .Type       = get_rand_type(min_type, max_type),
        .Value      = get_rand_val(min_val, max_val),
        .Status     = get_rand_status(min_status, max_status),
    };

    return rand_data;
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
