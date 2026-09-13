#ifndef DATA_H
#define DATA_H

typedef struct
{
    long Timestamp;
    long SensorId;
    char* Type;
    double Value;
    char* Status;
} Data;

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

#endif
