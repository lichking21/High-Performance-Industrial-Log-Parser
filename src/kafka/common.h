#ifndef KAFKA_COMMON_H
#define KAFKA_COMMON_H
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <librdkafka/rdkafka.h>

static volatile sig_atomic_t kafka_running = 1;
static void kafka_stop(int sig) { (void)sig; kafka_running = 0; }
static void kafka_signals(void)
{
    kafka_running = 1;
    signal(SIGINT, kafka_stop);
    signal(SIGTERM, kafka_stop);
}
static int kafka_option(rd_kafka_conf_t* conf, const char* key, const char* value)
{
    char error[512];
    if (rd_kafka_conf_set(conf, key, value, error, sizeof(error)) == RD_KAFKA_CONF_OK)
        return 0;
    fprintf(stderr, "%s: %s\n", key, error);
    return -1;
}
static int kafka_count(const char* text, size_t* count)
{
    if (!text || text[0] < '0' || text[0] > '9') return -1;
    char* end;
    errno = 0;
    unsigned long long value = strtoull(text, &end, 10);
    if (errno || *end || value > SIZE_MAX) return -1;
    *count = (size_t)value;
    return 0;
}
#endif
