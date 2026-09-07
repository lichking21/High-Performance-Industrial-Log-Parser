#ifndef GENERATOR_H
#define GENERATOR_H

typedef enum
{
  OK,
  WARN,
  ERR,
} Status;

typedef enum
{
    TEMPERATURE,
    PRESSURE,
    VIBRATION,
} Type;

typedef struct
{
    long Timestamp;
    long SensorId;
    Type Type;
    double Value;
    Status Status;
} Data;

Data NewData();

Data GenerateRandomData();

void print_data(Data data);

long get_rand_timestamp(long min_date, long max_date);
long get_rand_sensor_id(long min_id, long max_id);
Type get_rand_type(int min, int max);
double get_rand_val(double min, double max);
Status get_rand_status(int min, int max);

#endif
