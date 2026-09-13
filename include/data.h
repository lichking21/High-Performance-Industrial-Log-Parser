#ifndef DATA_H
#define DATA_H

#include <stdio.h>

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

typedef struct
{
    const char* ptr;
    size_t len;
} StringView;

#endif
