#ifndef READER_KAFKA_H
#define READER_KAFKA_H
#include <stddef.h>
/* A zero limit parses until SIGINT or SIGTERM. */
int kafka_parse(const char* brokers, const char* topic, const char* group, size_t limit);
#endif
