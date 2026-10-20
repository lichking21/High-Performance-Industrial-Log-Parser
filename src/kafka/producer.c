#include "producer.h"
#include "common.h"
struct KafkaProducer
{
    rd_kafka_t* client;
    rd_kafka_topic_t* topic;
    int failed;
};

static void delivered(rd_kafka_t* client, const rd_kafka_message_t* message, void* opaque)
{
    (void)client;

    if (message->err)
    {
        fprintf(stderr, "Delivery failed: %s\n", rd_kafka_err2str(message->err));
        *(int*)opaque = 1;
    }
}

KafkaProducer* kafka_producer_open(const char* brokers, const char* topic)
{
    KafkaProducer* producer = calloc(1, sizeof(KafkaProducer));
    if (producer == NULL) return NULL;
    char error[512];

    rd_kafka_conf_t* conf = rd_kafka_conf_new();
    if (kafka_option(conf, "bootstrap.servers", brokers) ||
        kafka_option(conf, "message.timeout.ms", "10000") ||
        kafka_option(conf, "enable.idempotence", "true"))
    {
        rd_kafka_conf_destroy(conf);
        free(producer);
        return NULL;
    }

    rd_kafka_conf_set_dr_msg_cb(conf, delivered);
    rd_kafka_conf_set_opaque(conf, &producer->failed);

    rd_kafka_t* client = rd_kafka_new(RD_KAFKA_PRODUCER, conf, error, sizeof(error));
    if (!client)
    {
        fprintf(stderr, "%s\n", error);
        rd_kafka_conf_destroy(conf);
        free(producer);
        return NULL;
    }

    rd_kafka_topic_t* destination = rd_kafka_topic_new(client, topic, NULL);
    if (!destination)
    {
        fprintf(stderr, "Topic: %s\n", rd_kafka_err2str(rd_kafka_last_error()));
        rd_kafka_destroy(client);
        free(producer);
        return NULL;
    }

    producer->client = client;
    producer->topic = destination;

    return producer;
}

int kafka_producer_write(KafkaProducer* producer, const char* message, size_t length)
{
    if (producer == NULL || message == NULL || producer->failed) return -1;

    while (rd_kafka_produce(producer->topic, RD_KAFKA_PARTITION_UA,
               RD_KAFKA_MSG_F_COPY, (void*)message, length, NULL, 0, NULL) == -1)
    {
        rd_kafka_resp_err_t err = rd_kafka_last_error();
        if (err != RD_KAFKA_RESP_ERR__QUEUE_FULL)
        {
            fprintf(stderr, "Produce: %s\n", rd_kafka_err2str(err));
            producer->failed = 1;
            return -1;
        }

        rd_kafka_poll(producer->client, 100);
        if (producer->failed) return -1;
    }

    rd_kafka_poll(producer->client, 0);
    return producer->failed ? -1 : 0;
}

int kafka_producer_close(KafkaProducer* producer)
{
    if (producer == NULL) return -1;

    rd_kafka_resp_err_t err = rd_kafka_flush(producer->client, 15000);
    if (err)
    {
        fprintf(stderr, "Flush: %s\n", rd_kafka_err2str(err));
        producer->failed = 1;
    }

    int result = producer->failed ? -1 : 0;
    rd_kafka_topic_destroy(producer->topic);
    rd_kafka_destroy(producer->client);
    free(producer);

    return result;
}
