#ifndef KAFKA_PRODUCER_H
#define KAFKA_PRODUCER_H

#include <stddef.h>

typedef struct KafkaProducer KafkaProducer;

KafkaProducer* kafka_producer_open(const char* brokers, const char* topic);
int kafka_producer_write(KafkaProducer* producer, const char* message, size_t length);
int kafka_producer_close(KafkaProducer* producer);

#endif
