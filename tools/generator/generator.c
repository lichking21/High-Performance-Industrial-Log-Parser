#include "generator.h"
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
        fclose(fp);
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
    int bytes = fprintf(
        fp,
        "%ld,%ld,%s,%f,%s\n",
        data.Timestamp, data.SensorId, data.Type, data.Value, data.Status
    );

    return bytes;
}

char* check_type(Type type)
{
    if (type == TEMPERATURE)       return "TEMPERATURE";
    else if (type == PRESSURE)     return "PRESSURE";
    else if (type == VIBRATION)    return "VIBRATION";

    return NULL;
}
char* check_status(Status status)
{
    if (status == OK)          return "OK";
    else if (status == WARN)   return "WARN";
    else if (status == ERR)    return "ERR";

    return NULL;
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

    Type t = get_rand_type(min_type, max_type);
    char* type = check_type(t);

    double value = get_rand_val(min_val, max_val);

    Status s = get_rand_status(min_status, max_status);
    char* status = check_status(s);

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
    printf("Date:       %ld\n", data.Timestamp);
    printf("Sensor ID:  %ld\n", data.SensorId);
    printf("Type:       %s\n",  data.Type);
    printf("Value:      %f\n",  data.Value);
    printf("Status:     %s\n",  data.Status);
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
