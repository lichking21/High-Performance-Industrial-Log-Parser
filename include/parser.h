#ifndef PARSER_H
#define PARSER_H

#include "data.h"

typedef int (*ParserFunc)(const char*);

typedef struct
{
    const char* name;
    ParserFunc func;
}ParserType;

int     parse_file(const char* filename, const char* parser);

Type    parse_type(size_t len, const char* str);
Status  parse_status(size_t len, const char* str);
long    parse_long(const char* begin, const char* end);

int     parse_line(const char* line, size_t len, Data* data);

#endif
