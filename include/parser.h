#ifndef PARSER_H
#define PARSER_H

#include "data.h"

int parse_file(const char* filename);

Type parse_type(size_t len, char* str);
Status parse_status(size_t len, char* str);
int parse_line(char* line, Data* data);

#endif
