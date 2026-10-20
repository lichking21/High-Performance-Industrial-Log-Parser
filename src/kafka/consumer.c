#include "consumer.h"
#include "common.h"

// Check every assigned partition: one EOF does not mean the whole topic is read.
static int consumer_at_end(rd_kafka_t* client, rd_kafka_topic_partition_list_t* ends)
{
    rd_kafka_topic_partition_list_t* partitions = NULL;
    rd_kafka_resp_err_t err = rd_kafka_assignment(client, &partitions);
    if (err) return -1;

    int finished = partitions->cnt > 0;
    err = rd_kafka_position(client, partitions);
    if (err)
    {
        rd_kafka_topic_partition_list_destroy(partitions);
        return -1;
    }

    for (int i = 0; i < partitions->cnt; i++)
    {
        rd_kafka_topic_partition_t* partition = &partitions->elems[i];
        int64_t low, high;
        err = rd_kafka_get_watermark_offsets(client, partition->topic,
                                             partition->partition, &low, &high);
        rd_kafka_topic_partition_t* end = rd_kafka_topic_partition_list_find(
            ends, partition->topic, partition->partition);
        int64_t position = partition->offset;
        // Empty partitions have no consumed-message position; use their EOF offset.
        if (position < 0 && end != NULL) position = end->offset;
        if (err || partition->err || high < 0 || position < high)
        {
            finished = 0;
            break;
        }
    }

    rd_kafka_topic_partition_list_destroy(partitions);
    return finished;
}

int kafka_consumer(const char* brokers, const char* topic, const char* group, size_t limit,
                   KafkaMessageHandler handler, void* context)
{
    if (!handler) return -1;

    char error[512];

    rd_kafka_conf_t* conf = rd_kafka_conf_new();
    if (kafka_option(conf, "bootstrap.servers", brokers) ||
        kafka_option(conf, "group.id", group) ||
        kafka_option(conf, "auto.offset.reset", "earliest") ||
        kafka_option(conf, "enable.partition.eof", "true") ||
        kafka_option(conf, "enable.auto.offset.store", "false"))
    {
        rd_kafka_conf_destroy(conf);
        return -1;
    }

    rd_kafka_t* client = rd_kafka_new(RD_KAFKA_CONSUMER, conf, error, sizeof(error));
    if (!client)
    {
        fprintf(stderr, "%s\n", error);
        rd_kafka_conf_destroy(conf);
        return -1;
    }

    int failed = rd_kafka_poll_set_consumer(client) != RD_KAFKA_RESP_ERR_NO_ERROR;
    rd_kafka_topic_partition_list_t* topics = rd_kafka_topic_partition_list_new(1);

    rd_kafka_topic_partition_list_add(topics, topic, RD_KAFKA_PARTITION_UA);
    rd_kafka_resp_err_t err = rd_kafka_subscribe(client, topics);

    rd_kafka_topic_partition_list_destroy(topics);

    if (err)
    {
        fprintf(stderr, "Subscribe: %s\n", rd_kafka_err2str(err));
        failed = 1;
    }

    size_t records = 0;
    rd_kafka_topic_partition_list_t* ends = rd_kafka_topic_partition_list_new(1);

    kafka_signals();

    while (!failed && kafka_running && (!limit || records < limit))
    {
        rd_kafka_message_t* message = rd_kafka_consumer_poll(client, 500);
        if (!message) continue;

        if (message->err == RD_KAFKA_RESP_ERR__PARTITION_EOF)
        {
            const char* name = rd_kafka_topic_name(message->rkt);
            rd_kafka_topic_partition_t* end = rd_kafka_topic_partition_list_find(
                ends, name, message->partition);
            if (end == NULL)
                end = rd_kafka_topic_partition_list_add(ends, name, message->partition);
            end->offset = message->offset;
            int finished = consumer_at_end(client, ends);
            rd_kafka_message_destroy(message);
            if (finished < 0)
            {
                fprintf(stderr, "(ERR) >> Failed to check Kafka partition positions\n");
                failed = 1;
                break;
            }
            if (finished) break;
            continue;
        }

        if (message->err)
        {
            fprintf(stderr, "Consume: %s\n", rd_kafka_message_errstr(message));
            failed = 1;
            rd_kafka_message_destroy(message);
            break;
        }

        if (handler(message->payload, message->len, context) != 0)
        {
            rd_kafka_message_destroy(message);
            failed = 1;
            break;
        }

        ++records;

        // Store the offset only after the handler has processed the message.
        rd_kafka_error_t* store_error = rd_kafka_offset_store_message(message);

        if (store_error)
        {
            fprintf(stderr, "Store offset: %s\n", rd_kafka_error_string(store_error));
            rd_kafka_error_destroy(store_error);
            failed = 1;
        }
        rd_kafka_message_destroy(message);
    }

    rd_kafka_topic_partition_list_destroy(ends);
    if (rd_kafka_consumer_close(client)) failed = 1;
    rd_kafka_destroy(client);

    return failed ? -1 : 0;
}
