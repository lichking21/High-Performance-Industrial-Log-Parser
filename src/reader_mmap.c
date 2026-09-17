#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>
#include <unistd.h>
#include "stats.h"
#include "data.h"
#include "parser.h"

int mmap_parse(const char* filename)
{
    if (filename == NULL)
    {
        printf("(ERR) >> Filename can't be null\n");
        return -1;
    }

    struct stat buffer;
    int         fd, status;
    size_t      file_size;

    fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        printf("(ERR) >> Failed to open file: %s\n", filename);
        return -1;
    }

    status = fstat(fd, &buffer);
    if (status == -1)
    {
        printf("(ERR) >> Failed to get file status: %s\n", filename);
        close(fd);
        return -1;
    }

    file_size = buffer.st_size;

    char* mapped = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED)
    {
        printf("(ERR) >> Mapping failed\n");
        return -1;
    }

    const char* line_begin    = mapped;
    const char* file_end      = mapped + file_size;

    Stats stats;
    size_t records      = 0;
    size_t corrupted    = 0;

    struct timespec start, end;
    double processing_time  = 0;
    double throughput       = 0;

    stats_init(&stats);
    timespec_get(&start, TIME_UTC);

    while (line_begin < file_end)
    {
        const char* line_end = memchr(line_begin, '\n', file_end - line_begin);
        if (line_end == NULL)
            line_end = file_end;

        Data data;
        records++;
        size_t line_len = line_end - line_begin;

        if (parse_line(line_begin, line_len, &data) != 0)
            corrupted++;

        stats_upd(&stats, &data, records, corrupted);

        line_begin = line_end + 1;
    }

    timespec_get(&end, TIME_UTC);

    processing_time = (double)(end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    throughput = ((double)file_size / (1024.0 * 1024.0)) / processing_time;
    upd_time(&stats, processing_time, throughput);

    stats_print(&stats);

    close(fd);
    return 0;
}
