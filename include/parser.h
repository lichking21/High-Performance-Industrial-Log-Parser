#ifndef PARSER_H
#define PARSER_H

#include "data.h"

int parse_file(const char* filename);
int parse_line(char* line, Data* data);

#endif
