#include "producer.h"
#include "common.h"
#include "generator.h"
#include <time.h>

static void delivered(rd_kafka_t* client, const rd_kafka_message_t* message, void* opaque)
{
    (void)client;

    if (message->err)
    {
        fprintf(stderr, "Delivery failed: %s\n", rd_kafka_err2str(message->err));
        *(int*)opaque = 1;
    }
}

int kafka_producer(const char* brokers, const char* topic, size_t count)
{
    int failed = 0;
    char error[512];

    rd_kafka_conf_t* conf = rd_kafka_conf_new();
    if (kafka_option(conf, "bootstrap.servers", brokers) ||
        kafka_option(conf, "message.timeout.ms", "10000") ||
        kafka_option(conf, "enable.idempotence", "true"))
    {
        rd_kafka_conf_destroy(conf);
        return -1;
    }

    rd_kafka_conf_set_dr_msg_cb(conf, delivered);
    rd_kafka_conf_set_opaque(conf, &failed);

    rd_kafka_t* client = rd_kafka_new(RD_KAFKA_PRODUCER, conf, error, sizeof(error));
    if (!client)
    {
        fprintf(stderr, "%s\n", error);
        rd_kafka_conf_destroy(conf);
        return -1;
    }

    rd_kafka_topic_t* destination = rd_kafka_topic_new(client, topic, NULL);
    if (!destination)
    {
        fprintf(stderr, "Topic: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
        rd_kafka_destroy(client);
        return -1;
    }

    kafka_signals();
    srand((unsigned)time(NULL));

    size_t queued = 0;

    while (kafka_running && !failed && queued < count)
    {
        Data data = GenerateRandomData();

        char line[256];

        int length = snprintf(line, sizeof(line), "%ld,%ld,%s,%f,%s\n",
            data.Timestamp, data.SensorId, type_to_str(data.Type),
            data.Value, status_to_str(data.Status));

        if (length < 0 || (size_t)length >= sizeof(line)) { failed = 1; break; }

        while (rd_kafka_produce(destination, RD_KAFKA_PARTITION_UA,
                   RD_KAFKA_MSG_F_COPY, line, (size_t)length, NULL, 0, NULL) == -1)
        {
            rd_kafka_resp_err_t err = rd_kafka_last_error();

            if (err != RD_KAFKA_RESP_ERR__QUEUE_FULL)
            {
                fprintf(stderr, "Produce: %s\n", rd_kafka_err2str(err));
                failed = 1;
                break;
            }

            rd_kafka_poll(client, 100);

            if (!kafka_running || failed) break;
        }

        if (!kafka_running || failed) break;

        ++queued;

        rd_kafka_poll(client, 0);
    }
    if (rd_kafka_flush(client, 15000) != RD_KAFKA_RESP_ERR_NO_ERROR) failed = 1;

    fprintf(stderr, "Queued %zu records; delivery %s\n", queued, failed ? "failed" : "complete");

    rd_kafka_topic_destroy(destination);
    rd_kafka_destroy(client);

    return failed ? -1 : 0;
}

int main(int argc, char** argv)
{
    size_t count;

    if (argc != 4 || kafka_count(argv[3], &count))
    {
        fprintf(stderr, "Usage: %s <brokers> <topic> <record-count>\n", argv[0]);
        return EXIT_FAILURE;
    }

    return kafka_producer(argv[1], argv[2], count) ? EXIT_FAILURE : EXIT_SUCCESS;
}
