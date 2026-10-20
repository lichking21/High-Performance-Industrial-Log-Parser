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

To generate a dataset, use the `--gen` flag and choose CSV or Kafka:

```bash
./build/app --gen -csv <file_name> <size> <B | KB | MB | GB>
./build/app --gen -kafka <brokers> <topic> <size> <B | KB | MB | GB>
```

Examples:

```bash
./build/app --gen -csv data.csv 1 GB
./build/app --gen -kafka localhost:9092 sensor-data 1 MB
```

Kafka sends one CSV record per message without a header. The size specifies the
message payload volume, not the number of records. Generation finishes after
Kafka confirms delivery. The last complete record can exceed the requested size.

The existing CSV syntax is also supported:

```bash
./build/app --gen data.csv 1 GB
```

## Data Parsing

To parse a generated dataset, use the `--parse` flag and choose a parser:

```bash
./build/app --parse <file_name> <-stdio | -mmap | -mpi | -openmpi>
./build/app --parse -kafka <brokers> <topic> <group-id> [record-limit]
```

Examples:

```bash
./build/app --parse data.csv -stdio
./build/app --parse data.csv -mmap
mpirun -np 4 ./build/app --parse data.csv -mpi
mpirun -np 4 ./build/app --parse data.csv -openmpi
./build/app --parse -kafka localhost:9092 sensor-data parser-group
./build/app --parse -kafka localhost:9092 sensor-data parser-group 1000
```

`-stdio` reads the file using standard I/O, `-mmap` uses memory mapping, and
`-mpi` parses the file using OpenMPI processes. `-openmpi` is an alias for `-mpi`.

Kafka parsing stops when all partitions assigned to this consumer reach their
current end or the optional record limit is reached. Omit the limit or use `0`
to read all available records. If producers keep adding messages, reading
continues until the consumer catches up or reaches the limit.

`group-id` identifies a consumer group and its saved reading position. Reusing
`parser-group` resumes from committed offsets. A new group starts from the
earliest available records. Consumers in the same group share partitions;
different groups read independently.

The following Kafka command is also supported:

```bash
./build/app --parse-kafka localhost:9092 sensor-data parser-group
```

After parsing, dataset statistics are collected using the following structure:

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
parser -stdio 
````text
Processing time:    4.06 s
Throughput:         252.24 MB/s
````
parser -mmap
````text
Processing time:    3.52 s
Throughput:         290.87 MB/s
````
parser -mpi
````text
Processing time:    1.43 s
Throughput:         717.37 MB/s
````

## Prerequirments

- C compiler with OpenMP support
- CMake 4.0 or newer
- OpenMPI library
- pkg-config
- librdkafka library and development headers
- Apache Kafka broker for Kafka generation and parsing

The broker must be reachable at the specified address, and the topic must exist
or topic auto-creation must be enabled.
