#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "parser.h"
#include "data.h"
#include "reader_mmap.h"
#include "reader_stdio.h"

const ParserType parsers[] = {
    {"-stdio", stdio_parse},
    {"-mmap",  mmap_parse}
};

int parse_file(const char* filename, const char* parser)
{
    for (size_t i = 0; i < sizeof(parsers) / sizeof(parsers[0]); ++i)
    {
        if (strcmp(parser, parsers[i].name) == 0)
            return parsers[i].func(filename);
    }

    printf("(ERR) >> Unknown parser: %s\n", parser);
    return -1;
}

Type parse_type(size_t len, const char* str)
{
    if (len == 11) return TEMPERATURE;
    else if (len == 8) return PRESSURE;
    else if (len == 9) return VIBRATION;

    return -1;
}
Status parse_status(size_t len, const char* str)
{
    if (len == 2) return OK;
    else if (len == 4) return WARN;
    else if (len == 3) return ERR;

    return -1;
}
long parse_long(const char* begin, const char* end)
{
    if (begin == NULL || end == NULL || begin >= end) return -1;

    long res = 0;

    for (const char* p = begin; p < end; p++)
    {
        if (*p < '0' || *p > '9') return -1;

        res = res * 10 + (*p - '0');
    }

    return res;
}

int parse_line(const char* line, size_t len, Data* data)
{
    if (line == NULL || data == NULL || len == 0) return -1;

    const char* ptr   = line;
    const char* line_end   = line + len;
    char* end;

    // Timestamp
    const char* comma = memchr(ptr, ',', line_end - ptr);
    if (comma == NULL) return -1;

    data->Timestamp = parse_long(ptr, comma);
    ptr = comma + 1;

    // SensorId
    comma = memchr(ptr, ',', line_end - ptr);
    if (comma == NULL) return -1;

    data->SensorId = parse_long(ptr, comma);
    ptr = comma + 1;

    // Type
    comma = memchr(ptr, ',', line_end - ptr);
    if (comma == NULL) return -1;

    size_t type_len = comma - ptr;
    data->Type = parse_type(type_len, ptr);
    ptr = comma + 1;

    // Value
    comma = memchr(ptr, ',', line_end - ptr);
    if (comma == NULL) return -1;

    data->Value = strtod(ptr, &end);
    if (end != comma) return -1;
    ptr = comma + 1;

    // Status
    const char* status_end = line_end;

    while (status_end > ptr && (status_end[-1] == '\n' || status_end[-1] == '\r'))
        status_end--;

    size_t status_len = status_end - ptr;
    if (status_len == 0) return -1;
    data->Status = parse_status(status_len, ptr);

    return 0;
}
