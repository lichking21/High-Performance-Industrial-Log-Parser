#ifndef KAFKA_CONSUMER_H
#define KAFKA_CONSUMER_H

#include <stddef.h>

// Payload is borrowed and valid only during the callback.
// Return zero after processing, nonzero to stop without storing its offset.
typedef int (*KafkaMessageHandler)(const void* payload, size_t length, void* context);
// A zero limit consumes until SIGINT or SIGTERM.

int kafka_consumer(const char* brokers, const char* topic, const char* group,
                   size_t limit, KafkaMessageHandler handler, void* context);

#endif
