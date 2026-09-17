#include <stdio.h>
#include <time.h>
#include "stats.h"
#include "data.h"
#include "parser.h"

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
    size_t records = 0;
    size_t corrupted = 0;

    struct timespec start, end;
    double processing_time = 0;
    double throughput = 0;

    // get file size
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    rewind(fp);

    if (size < 0)
    {
        fprintf("(ERR) >> File size can't be less than 0\n");
        fclose(fp);
        return -1;
    }

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
