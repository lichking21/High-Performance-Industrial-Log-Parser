#include <mpi.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

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

    close(*fd);
    return 0;
}

static int map_file(int fd, size_t file_size, char* map)
{
    if (fd == -1 || file_size == 0 || map == NULL)
        return -1;

    map = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (map == MAP_FAILED)
    {
        printf("(ERR) >> Mapping failed\n");
        close(fd);
        return -1;
    }

    return 0;
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

    if (map_file(fd, file_size, map) != 0)
    {
        MPI_Finalize();
        return -1;
    }

    size_t chunk_size = file_size / world_size;
    size_t start_offset = proc * chunk_size;
    char* start = map + start_offset;

    MPI_Finalize();
    return 0;
}
