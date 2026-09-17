#ifndef PARSER_H
#define PARSER_H

#include "data.h"

int     parse_file(const char* filename);

Type    parse_type(size_t len, const char* str);
Status  parse_status(size_t len, const char* str);
int     parse_line(const char* line, size_t len, Data* data);

#endif
