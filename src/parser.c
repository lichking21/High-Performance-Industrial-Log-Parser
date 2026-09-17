#include "parser.h"
#include "data.h"
#include "stats.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

int parse_file(const char* filename)
{


    return 0;
}

Type parse_type(size_t len, const char* str)
{
    if (len == 11 && memcmp(str, "TEMPERATURE", 11) == 0) return TEMPERATURE;
    else if (len == 8 && memcmp(str, "PRESSURE", 8) == 0) return PRESSURE;
    else if (len == 9 && memcmp(str, "VIBRATION", 9) == 0) return VIBRATION;

    return -1;
}
Status parse_status(size_t len, const char* str)
{
    if (len == 2 && memcmp(str, "OK", 2) == 0) return OK;
    else if (len == 4 && memcmp(str, "WARN", 4) == 0) return WARN;
    else if (len == 3 && memcmp(str, "ERR", 3) == 0) return ERR;

    return -1;
}

int parse_line(const char* line, size_t len, Data* data)
{
    if (line == NULL || data == NULL || len == 0)
        return -1;

    const char* str   = line;
    const char* line_end   = line + len;
    char* end;


    // Timestamp
    const char* comma = memchr(str, ',', line_end - str);
    if (comma == NULL)
        return -1;

    data->Timestamp = strtol(str, &end, 10);
    if (end != comma)
        return -1;

    str = comma + 1;

    // SensorId
    comma = memchr(str, ',', line_end - str);
    if (comma == NULL)
        return -1;

    data->SensorId = strtol(str, &end, 10);
    if (end != comma)
        return -1;

    str = comma + 1;

    // Type
    comma = memchr(str, ',', line_end - str);
    if (comma == NULL)
        return -1;

    size_t type_len = comma - str;

    data->Type = parse_type(type_len, str);

    str = comma + 1;

    // Value
    comma = memchr(str, ',', line_end - str);
    if (comma == NULL)
        return -1;

    data->Value = strtod(str, &end);
    if (end != comma)
        return -1;

    str = comma + 1;

    // Status
    const char* status_end = line_end;

    while (status_end > str && (status_end[-1] == '\n' || status_end[-1] == '\r'))
        status_end--;

    size_t status_len = status_end - str;

    if (status_len == 0)
        return -1;

    data->Status = parse_status(status_len, str);


    return 0;
}
