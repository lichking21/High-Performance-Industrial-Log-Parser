#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "generator.h"

size_t convert_string(const char* str)
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

int main(int argc, char** argv)
{
    srand(time(NULL));

    if (argc < 5)
    {
        printf("(ERR) >> Not enough arguments\n");
        return -1;
    }
    else if (argc > 5)
    {
        printf("(ERR) >> Too many arguments\n");
        return -1;
    }

    char* flag = argv[1];
    size_t target_size = convert_string(argv[2]);
    char* size_unit = argv[3];
    char* file_name = argv[4];

    printf("(LOG) >> %s\n", flag);
    printf("(LOG) >> %ld\n", target_size);
    printf("(LOG) >> %s\n", size_unit);
    printf("(LOG) >> %s\n", file_name);

    if (strcmp(flag, "--gen") == 0)
    {
        if (strcmp(size_unit, "B") == 0)         target_size *= 1;
        else if (strcmp(size_unit, "KB") == 0)   target_size *= 1024;
        else if (strcmp(size_unit, "MB") == 0)   target_size *= 1024 * 1024;
        else if (strcmp(size_unit, "GB") == 0)   target_size *= 1024 * 1024 * 1024;
        else
        {
            printf("(ERR) >> Invalid size unit.\n Try to use B, KB, MB, GB\n");
            return -1;
        }

        generate_csv(file_name, target_size);
    }
    else
    {
        printf("(ERR) >> Invalid flag.\n Try to use: \n");
        printf("\t--gen - generate file <file_size> <size_unit> <file_name>\n");
        return -1;
    }

    return 0;
}
