#ifndef KAFKA_PRODUCER_H
#define KAFKA_PRODUCER_H

#include <stddef.h>

int kafka_producer(const char* brokers, const char* topic, size_t count);

#endif
