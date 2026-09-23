# Industrial Data Parser

A simple C program for generating and parsing large industrial datasets in CSV format.

The program can generate raw industrial data using the following structure:

```c
typedef struct
{
    long Timestamp;
    long SensorId;
    Type Type;       // TEMPERATURE, PRESSURE, VIBRATION
    double Value;
    Status Status;   // OK, WARN, ERR
} Data;
```

## Data Generation

To generate a `.csv` dataset, use the `--gen` flag:

```bash
./parser --gen <file_name> <file_size> <size_unit>
```

Example:

```bash
./parser --gen data.csv 1 GB
```

## Data Parsing

To parse a generated dataset, use the `--parse` flag:

```bash
./parser --parse <file_name>
```

Example:

```bash
./parser --parse data.csv
```

After parsing, file statistics are collected using the following structure:

```c
typedef struct
{
    size_t total_records;
    size_t corrupted_records;

    ValueStats temperature;
    ValueStats pressure;
    ValueStats vibration;

    size_t status_ok;
    size_t status_warn;
    size_t status_err;

    double processing_time;
    double throughput;
} Stats;
```

## Result Example

```text
Records:            24341622
Corrupted records:  0

Temperature:
  Count:    8112642
  MAX:      10000.00
  MIN:      1.00
  SUM:      40571030336.13
  AVG:      5000.96

Pressure:
  Count:    8113826
  MAX:      10000.00
  MIN:      1.00
  SUM:      40590227514.74
  AVG:      5002.60

Vibration:
  Count:    8115154
  MAX:      10000.00
  MIN:      1.00
  SUM:      40579490105.13
  AVG:      5000.46

Status:
  OK:       8114527
  WARN:     8112499
  ERR:      8114596

Processing time:    4.09 s
Throughput:         250.38 MB/s
```


## Benchmarks

### 1 GB CSV file
```bash
-stdio
```
````text
Processing time:    4.06 s
Throughput:         252.24 MB/s
````

```bash
-mmap
```
````text
Processing time:    3.52 s
Throughput:         290.87 MB/s
````

## Prerequirments
OpenMPI library
