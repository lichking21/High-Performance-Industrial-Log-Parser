#include "data.h"
#include <stdio.h>

void print_data(Data data)
{
    const char* type = type_to_str(data.Type);
    const char* status = status_to_str(data.Status);

    //printf("Date,\tSensorID,\tType,\tValue,\tStatus\n");
    printf("%ld,\t%ld,\t%s,\t%f,\t%s\n",
        data.Timestamp, data.SensorId, type, data.Value, status);
}

const char* type_to_str(Type type)
{
    switch(type)
    {
        case TEMPERATURE:
            return "TEMPERATURE";
        case PRESSURE:
            return "PRESSURE";
        case VIBRATION:
            return "VIBRATION";
    }

    return "UNKNOWN";
}
const char* status_to_str(Status status)
{
    switch(status)
    {
        case OK:
            return "OK";
        case WARN:
            return "WARN";
        case ERR:
            return "ERR";
    }

    return "UNKNOWN";
}
