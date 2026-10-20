#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "generator.h"
#include "parser.h"
#include "reader_kafka.h"
#include <errno.h>

static int execute_kafka_parse(int argc, char** argv, int first)
{
    size_t limit = 0;
    if (argc != first + 3 && argc != first + 4)
    {
        fprintf(stderr, "Usage: %s --parse -kafka <brokers> <topic> <group-id> [record-limit]\n", argv[0]);
        return -1;
    }
    if (argc == first + 4)
    {
        char* end;
        errno = 0;
        uintmax_t value = strtoumax(argv[first + 3], &end, 10);
        if (argv[first + 3][0] < '0' || argv[first + 3][0] > '9' || *end || errno || value > SIZE_MAX)
        {
            fprintf(stderr, "(ERR) >> Invalid Kafka record limit\n");
            return -1;
        }
        limit = (size_t)value;
    }
    return kafka_parse(argv[first], argv[first + 1], argv[first + 2], limit);
}

static int execute_parse(int argc, char** argv)
{
    if (argc >= 3 && strcmp(argv[2], "-kafka") == 0)
    {
        return execute_kafka_parse(argc, argv, 3);
    }
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s --parse <file> <-stdio | -mmap | -mpi | -openmpi>\n", argv[0]);
        fprintf(stderr, "       %s --parse -kafka <brokers> <topic> <group-id> [record-limit]\n", argv[0]);
        return -1;
    }

    return parse_file(argv[2], argv[3]);
}

static size_t convert_string(const char* str)
{
    char* endptr;

    errno = 0;
    uintmax_t val = strtoumax(str, &endptr, 10);

    if (str[0] < '0' || str[0] > '9' || str == endptr || *endptr || errno)
    {
        printf("(ERR) >> There is no digits in passed arguments\n");
        return -1;
    }

    if (val > SIZE_MAX)
    {
        printf("(ERR) >> Value is bigger than SIZE_MAX\n");
        return -1;
    }

    size_t res = (size_t)val;
    printf("(LOG) >> Convertation was successfull\n");

    return res;
}

typedef struct
{
    const char* name;
    size_t multiplier;
} Unit;
static const Unit units[4] = {
    {"B", 1},
    {"KB", 1024ULL},
    {"MB", 1024ULL * 1024},
    {"GB", 1024ULL * 1024 * 1024},
};

static int execute_gen(int argc, char** argv)
{
    int kafka = argc >= 3 && strcmp(argv[2], "-kafka") == 0;
    int csv = argc >= 3 && strcmp(argv[2], "-csv") == 0;
    int size_index = kafka ? 5 : (csv ? 4 : 3);

    if (argc != size_index + 2)
    {
        printf("Try to use: %s --gen -csv <file> <size> <B | KB | MB | GB>\n", argv[0]);
        printf("            %s --gen -kafka <brokers> <topic> <size> <B | KB | MB | GB>\n", argv[0]);
        return -1;
    }

    size_t target_size = convert_string(argv[size_index]);
    if (target_size == SIZE_MAX) return -1;

    const char* unit = argv[size_index + 1];
    size_t multiplier = 0;
    for (size_t i = 0; i < sizeof(units) / sizeof(units[0]); i++)
    {
        if (strcmp(unit, units[i].name) == 0)
        {
            multiplier = units[i].multiplier;
            break;
        }
    }

    if (multiplier == 0)
    {
        printf("(ERR) >> Invalid size unit. Try to use B, KB, MB, GB\n");
        return -1;
    }

    if (target_size > SIZE_MAX / multiplier)
    {
        printf("(ERR) > File size is too large\n");
        return -1;
    }

    if (kafka)
        return generate_kafka(argv[3], argv[4], target_size * multiplier);

    const char* filename = argv[csv ? 3 : 2];
    return generate_csv(filename, target_size * multiplier);
}

int execute(int argc, char** argv)
{
    srand((unsigned)time(NULL));

    if (argc < 2)
    {
        printf("Usage:\n");
        printf("\t%s --parse -kafka <brokers> <topic> <group-id> [record-limit]\n", argv[0]);
        printf("\t%s --parse <file> <-stdio | -mmap | -mpi | -openmpi>\n", argv[0]);
        printf("\t%s --gen -csv <file> <size> <B | KB | MB | GB>\n", argv[0]);
        printf("\t%s --gen -kafka <brokers> <topic> <size> <B | KB | MB | GB>\n", argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "--parse-kafka") == 0)
        return execute_kafka_parse(argc, argv, 2);

    if (strcmp(argv[1], "--parse") == 0)
        return execute_parse(argc, argv);

    if (strcmp(argv[1], "--gen") == 0)
        return execute_gen(argc, argv);

    printf("(ERR) >> Unknown command: %s\n", argv[1]);
    return -1;
}
