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

int generate_kafka(const char* brokers, const char* topic, size_t target_size)
{
    KafkaProducer* producer = kafka_producer_open(brokers, topic);
    if (producer == NULL)
    {
        printf("(ERR) >> Failed to open Kafka producer\n");
        return -1;
    }

    size_t total_size = 0;

    while (total_size < target_size)
    {
        Data data = GenerateRandomData();
        int written = write_to_kafka(data, producer);

        if (written < 0)
        {
            printf("(ERR) >> Failed to write to Kafka\n");
            kafka_producer_close(producer);
            return -1;
        }

        if ((size_t)written >= target_size - total_size)
        {
            total_size = target_size;
            break;
        }

        total_size += (size_t)written;
    }

    if (kafka_producer_close(producer) != 0) return -1;

    printf(">> Data was successfully written to Kafka topic %s\n", topic);
    return 0;
}

int write_to_kafka(Data data, KafkaProducer* producer)
{
    char line[256];
    int bytes = snprintf(
        line, sizeof(line),
        "%ld,%ld,%s,%f,%s\n",
        data.Timestamp, data.SensorId, type_to_str(data.Type),
        data.Value, status_to_str(data.Status)
    );

    if (bytes < 0 || (size_t)bytes >= sizeof(line)) return -1;

    if (kafka_producer_write(producer, line, (size_t)bytes) != 0) return -1;

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
