#include "parser.h"
#include "data.h"
#include "stats.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define MAXBUFF 1000

int parse_file(const char* filename)
{
    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("(ERR) >> Failed to open file\n");
        return -1;
    }

    char line_buff[MAXBUFF];

    Stats stats;
    size_t records = 0, corrupted = 0;

    struct timespec start, end;
    double processing_time = 0, throughput = 0;

    // get file size
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    // skip header
    if (fgets(line_buff, MAXBUFF, fp) == NULL)
    {
        fclose(fp);
        return -1;
    }

    // initialize Stats struct
    stats_init(&stats);

    // mark the time
    timespec_get(&start, TIME_UTC);

    // parse file
    while(fgets(line_buff, MAXBUFF, fp) != NULL)
    {
        Data data;
        records += 1;

        if (parse_line(line_buff, &data) != 0)
        {
            corrupted += 1;
            continue;
        }

        if (stats_upd(&stats, &data, records, corrupted) != 0)
            continue;
    }

    timespec_get(&end, TIME_UTC);

    if (ferror(fp))
    {
        perror("(ERR) >> Failed while reading file\n");
        fclose(fp);
        return -1;
    }

    processing_time = (double)(end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    throughput = ((double)size / (1024.0 * 1024.0)) / processing_time;
    upd_time(&stats, processing_time, throughput);

    printf(">> File successfully parsed!\n");
    stats_print(&stats);

    fclose(fp);
    return 0;
}

Type parse_type(size_t len, char* str)
{
    if (len == 11 && memcmp(str, "TEMPERATURE", 11) == 0) return TEMPERATURE;
    else if (len == 8 && memcmp(str, "PRESSURE", 8) == 0) return PRESSURE;
    else if (len == 9 && memcmp(str, "VIBRATION", 9) == 0) return VIBRATION;

    return -1;
}
Status parse_status(size_t len, char* str)
{
    if (len == 2 && memcmp(str, "OK", 2) == 0) return OK;
    else if (len == 4 && memcmp(str, "WARN", 4) == 0) return WARN;
    else if (len == 3 && memcmp(str, "ERR", 3) == 0) return ERR;

    return -1;
}

int parse_line(char* line, Data* data)
{
    if (line == NULL || data == NULL) return -1;

    char* str = line;
    char* end;

    // Timestamp
    data->Timestamp = strtol(str, &end, 10);
    if (*end != ',')
        return -1;

    str = end + 1;

    // SensorId
    data->SensorId = strtol(str, &end, 10);
    if (*end != ',')
        return -1;

    str = end + 1;

    // Type
    char* comma = strchr(str, ',');
    if (comma == NULL)
        return -1;

    size_t type_len = comma - str;

    data->Type = parse_type(type_len, str);

    str = comma + 1;

    // Value
    data->Value = strtod(str, &end);
    if (*end != ',')
        return -1;

    str = end + 1;

    // Status
    size_t status_len = strcspn(str, "\r\n");
    data->Status = parse_status(status_len, str);

    return 0;
}
