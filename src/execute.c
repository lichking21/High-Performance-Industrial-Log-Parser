#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "generator.h"
#include "parser.h"

static int execute_parse(int argc, char** argv)
{
    if (argc != 4)
    {
        printf("Try to use: %s --parse <file> <-stdio | -mmap>\n", argv[0]);
        return -1;
    }

    const char* filename    = argv[2];
    const char* parser      = argv[3];

    return parse_file(filename, parser);
}

static size_t convert_string(const char* str)
{
    char* endptr;

    uintmax_t val = strtoumax(str, &endptr, 10);

    if (str == endptr)
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
    if (argc != 5)
    {
        printf("Try to use: %s --gen <file_name> <file_size> <size_unit>\n", argv[0]);
        return -1;
    }

    const char* filename = argv[2];
    size_t target_size = convert_string(argv[3]);
    if (target_size == SIZE_MAX) return -1;

    char* unit = argv[4];
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

    if (target_size > SIZE_MAX)
    {
        printf("(ERR) > File size is too large\n");
        return -1;
    }

    return generate_csv(filename, target_size * multiplier);
}

int execute(int argc, char** argv)
{
    srand((unsigned)time(NULL));

    if (argc < 2)
    {
        printf("Usage:\n");
        printf("\t%s --parse <file> <-stdio | -mmap>\n", argv[0]);
        printf("\t%s --gen <file> <size> <B | KB | MB | GB>\n", argv[0]);
        return -1;
    }

    if (strcmp(argv[1], "--parse") == 0)
        return execute_parse(argc, argv);

    if (strcmp(argv[1], "--gen") == 0)
        return execute_gen(argc, argv);

    printf("(ERR) >> Unknown command: %s\n", argv[1]);
    return -1;
}
