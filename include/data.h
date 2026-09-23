#ifndef DATA_H
#define DATA_H

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

void print_data(Data data);
const char* type_to_str(Type type);
const char* status_to_str(Status status);

#endif
