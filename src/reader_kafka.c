#include "reader_kafka.h"
#include "kafka/consumer.h"
#include "parser.h"
#include "stats.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct
{
    Stats stats;
    size_t bytes;
} KafkaParseContext;

static int parse_message(const void* payload, size_t length, void* opaque)
{
    KafkaParseContext* context = opaque;

    // parse_line uses strtod, so give it a terminated copy of the payload.
    if (length == SIZE_MAX) return -1;

    char* line = malloc(length + 1);

    if (!line) return -1;

    if (length) memcpy(line, payload, length);

    line[length] = '\0';

    Data data;
    Stats* stats = &context->stats;
    ++stats->total_records;
    context->bytes += length;
    if (memchr(line, '\0', length) || parse_line(line, length, &data) ||
        data.Timestamp < 0 || data.SensorId < 0 ||
        (unsigned)data.Type > VIBRATION || (unsigned)data.Status > ERR)
        ++stats->corrupted_records;
    else
        stats_upd(stats, &data, stats->total_records, stats->corrupted_records);
    free(line);
    return 0;
}

int kafka_parse(const char* brokers, const char* topic, const char* group, size_t limit)
{
    KafkaParseContext context = {0};

    stats_init(&context.stats);

    struct timespec start, end;
    timespec_get(&start, TIME_UTC);

    int result = kafka_consumer(brokers, topic, group, limit, parse_message, &context);
    timespec_get(&end, TIME_UTC);
    double seconds = (double)(end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    upd_time(&context.stats, seconds,
             seconds > 0 ? context.bytes / (1024.0 * 1024.0) / seconds : 0);

    stats_print(&context.stats);
    return result;
}
