#include "parser.h"
#include "data.h"
#include <string.h>
#include <stdio.h>

#define MAXBUFF 10000

int parse_file(const char* filename)
{
    FILE* fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("(ERR) >> Failed to open file\n");
        return -1;
    }

    char line_buff[MAXBUFF];

    while(fgets(line_buff, MAXBUFF, fp) != NULL)
    {
        parse_line(line_buff, NULL);
    }

    if (ferror(fp))
    {
        perror("(ERR) >> Failed while reading file\n");
        fclose(fp);
        return -1;
    }

    fclose(fp);

    return 0;
}

int parse_line(char* line, Data* data)
{
    if (line == NULL)
    {
        printf("(ERR) >> Parsing line can't be NULL\n");
        return -1;
    }

    if (data == NULL)
    {
        printf("(ERR) >> Parsing data can't be NULL\n");
        return -1;
    }

    char* token = strtok(line, ",");

    while(token != NULL)
    {
        printf("Token: %s\n", token);
        token = strtok(NULL, ",");
    }

    return 0;
}
