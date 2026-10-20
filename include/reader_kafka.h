#ifndef READER_KAFKA_H
#define READER_KAFKA_H
#include <stddef.h>
/* A zero limit parses all available records; otherwise stop at the limit or EOF. */
int kafka_parse(const char* brokers, const char* topic, const char* group, size_t limit);
#endif
