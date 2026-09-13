#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdio.h>
#include "data.h"

Data NewData();

Data GenerateRandomData();

void print_data(Data data);

int generate_csv(const char* filename, size_t target_size);
int write_to_csv(Data data, FILE* fp);

long get_rand_timestamp(long min_date, long max_date);
long get_rand_sensor_id(long min_id, long max_id);
Type get_rand_type(int min, int max);
double get_rand_val(double min, double max);
Status get_rand_status(int min, int max);

#endif
