#include <mpi.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "data.h"
#include "parser.h"
#include "stats.h"

#define WORLD MPI_COMM_WORLD
#define ROOT 0

static int get_file_size(const char* filename, size_t* file_size, int* fd)
{
    struct stat buf;
    *fd = (int)open(filename, O_RDONLY);

    if (*fd == -1)
    {
        printf("(ERR) >> Failed to open: %s\n", filename);
        return -1;
    }
    if (fstat(*fd, &buf) == -1)
    {
        printf("(ERR) >> Failed to get file status: %s\n", filename);
        close(*fd);
        return -1;
    }

    *file_size = (size_t)buf.st_size;

    return 0;
}

static char* map_file(int fd, size_t file_size)
{
    if (fd == -1 || file_size == 0)
        return NULL;

    char* map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED)
    {
        printf("(ERR) >> Mapping failed\n");
        return NULL;
    }

    return map;
}

int mpi_parse(const char* filename)
{
    // Initialize MPI
    if (MPI_Init(NULL, NULL) != MPI_SUCCESS)
        return -1;

    int world_size, proc;
    MPI_Comm_size(WORLD, &world_size);
    MPI_Comm_rank(WORLD, &proc);

    size_t file_size;
    int fd;
    char* map;

    if (get_file_size(filename, &file_size, &fd) != 0)
    {
        MPI_Finalize();
        return -1;
    }

    map = map_file(fd, file_size);
    if (map == NULL)
    {
        close(fd);
        MPI_Finalize();
        return -1;
    }
    close(fd);

    // calc chunk's start line and end line for each process
    size_t chunk_size       = file_size / world_size;
    size_t begin_offset     = proc * chunk_size;
    size_t end_offset       = (proc == world_size - 1) ? file_size : (proc + 1) * chunk_size;

    char* line_begin     = map + begin_offset;
    char* line_end       = map + end_offset;

    if (proc == ROOT)
    {
        char* header_end = memchr(map, '\n', file_size);

        if (header_end == NULL)
        {
            printf("(ERR) >> File doesn't contain data\n");
            munmap(map, file_size);
            return -1;
        }

        line_begin = header_end + 1;
    }
    else
    {
        if (begin_offset > 0 && map[begin_offset - 1] != '\n')
        {
            char* first_line_end = memchr(line_begin, '\n', file_size - begin_offset);

            if (first_line_end == NULL)
            {
                munmap(map, file_size);
                MPI_Finalize();
                return -1;
            }

            line_begin = first_line_end + 1;
        }
    }

    Stats local_stats;
    Stats stats;

    size_t records = 0, corrupted = 0;
    double start = 0, end = 0;

    stats_init(&local_stats);
    stats_init(&stats);

    MPI_Barrier(WORLD);
    start = MPI_Wtime();
    while(line_begin < line_end)
    {
        char* local_line_end = memchr(line_begin, '\n', (map + file_size) - line_begin);
        if (local_line_end == NULL)
            local_line_end = map + file_size;

        Data local_data;
        records++;

        size_t line_len = local_line_end - line_begin;

        if (parse_line(line_begin, line_len, &local_data) != 0)
            corrupted++;
        else
        {
            upd_types(&local_stats, &local_data);
            upd_statuses(&local_stats, &local_data);
        }

        if (local_line_end == map + file_size)
            break;

        line_begin = local_line_end + 1;
    }

    size_t total_records    = 0;
    size_t total_corrupted  = 0;

    // Temperature
    size_t t_total_count    = 0;
    double t_total_sum      = 0;
    double t_total_min      = 0;
    double t_total_max      = 0;

    // Pressure
    size_t p_total_count    = 0;
    double p_total_sum      = 0;
    double p_total_min      = 0;
    double p_total_max      = 0;

    // Vibration
    size_t v_total_count    = 0;
    double v_total_sum      = 0;
    double v_total_min      = 0;
    double v_total_max      = 0;

    size_t total_ok         = 0;
    size_t total_warn       = 0;
    size_t total_err        = 0;

    double total_time       = 0;

    MPI_Reduce(&records, &total_records, 1, MPI_UNSIGNED_LONG, MPI_SUM, ROOT, WORLD);
    MPI_Reduce(&corrupted, &total_corrupted, 1, MPI_UNSIGNED_LONG, MPI_SUM, ROOT, WORLD);

    MPI_Reduce(&local_stats.temperature.count, &t_total_count, 1, MPI_UNSIGNED_LONG, MPI_SUM, ROOT, WORLD);
    MPI_Reduce(&local_stats.temperature.sum, &t_total_sum, 1, MPI_DOUBLE, MPI_SUM, ROOT, WORLD);
    MPI_Reduce(&local_stats.temperature.min, &t_total_min, 1, MPI_DOUBLE, MPI_MIN, ROOT, WORLD);
    MPI_Reduce(&local_stats.temperature.max, &t_total_max, 1, MPI_DOUBLE, MPI_MAX, ROOT, WORLD);

    MPI_Reduce(&local_stats.pressure.count, &p_total_count, 1, MPI_UNSIGNED_LONG, MPI_SUM, ROOT, WORLD);
    MPI_Reduce(&local_stats.pressure.sum, &p_total_sum, 1, MPI_DOUBLE, MPI_SUM, ROOT, WORLD);
    MPI_Reduce(&local_stats.pressure.min, &p_total_min, 1, MPI_DOUBLE, MPI_MIN, ROOT, WORLD);
    MPI_Reduce(&local_stats.pressure.max, &p_total_max, 1, MPI_DOUBLE, MPI_MAX, ROOT, WORLD);

    MPI_Reduce(&local_stats.vibration.count, &v_total_count, 1, MPI_UNSIGNED_LONG, MPI_SUM, ROOT, WORLD);
    MPI_Reduce(&local_stats.vibration.sum, &v_total_sum, 1, MPI_DOUBLE, MPI_SUM, ROOT, WORLD);
    MPI_Reduce(&local_stats.vibration.min, &v_total_min, 1, MPI_DOUBLE, MPI_MIN, ROOT, WORLD);
    MPI_Reduce(&local_stats.vibration.max, &v_total_max, 1, MPI_DOUBLE, MPI_MAX, ROOT, WORLD);

    MPI_Reduce(&local_stats.status_ok, &total_ok, 1, MPI_UNSIGNED_LONG, MPI_SUM, ROOT, WORLD);
    MPI_Reduce(&local_stats.status_warn, &total_warn, 1, MPI_UNSIGNED_LONG, MPI_SUM, ROOT, WORLD);
    MPI_Reduce(&local_stats.status_err, &total_err, 1, MPI_UNSIGNED_LONG, MPI_SUM, ROOT, WORLD);

    end = MPI_Wtime();
    double local_time = end - start;
    MPI_Reduce(&local_time, &total_time, 1, MPI_DOUBLE, MPI_MAX, ROOT, WORLD);

    if (proc == ROOT)
    {
        stats.total_records         = total_records;
        stats.corrupted_records     = total_corrupted;

        stats.temperature.count     = t_total_count;
        stats.temperature.sum       = t_total_sum;
        stats.temperature.min       = t_total_min;
        stats.temperature.max       = t_total_max;

        stats.pressure.count        = p_total_count;
        stats.pressure.sum          = p_total_sum;
        stats.pressure.min          = p_total_min;
        stats.pressure.max          = p_total_max;

        stats.vibration.count       = v_total_count;
        stats.vibration.sum         = v_total_sum;
        stats.vibration.min         = v_total_min;
        stats.vibration.max         = v_total_max;

        stats.status_ok             = total_ok;
        stats.status_warn           = total_warn;
        stats.status_err            = total_err;

        stats.processing_time       = total_time;

        double throughput = ((double)file_size / (1024.0 * 1024.0)) / total_time;
        stats.throughput            = throughput;

        stats_print(&stats);
    }

    munmap(map, file_size);
    MPI_Finalize();
    return 0;
}
