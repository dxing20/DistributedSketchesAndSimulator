# Distributed Sketches and Switch Simulator

A network switch simulator along with distributed algorithm and data structures.

## Requirements
- Docker

## Setup
Build and start container
```shell
bash scripts/container.sh
```
Build cpp and run main
```shell
./scripts/makerun.sh
```

## Features

### Generate Traces

```
./scripts/makerun.sh -g "<distribution>:<args>"
```

| Distribution | Args | Example |
| ------ | ------ | ------ |
| zipf | \<seed\>:\<alpha\>:\<N\>:\<Size\> | `./scripts/makerun.sh -g "zipf:1:0.8:10000:10000"` |


This will generate 4 files:
| Location | File Type | Description |
| ------ | ------ | ------ |
| `trace/<distribution>_stream/*.bin` | binary | consecutive uint32 random values from the distribution. |
| `trace/<distribution>_stream/*.txt` | text | aggregated frequency of the random values in human readable form. |
| `trace/*.bin` | binary | consecutive 13 byte random values mapped from the random value stream. |
| `trace/*.txt` | text | aggregated frequency of flows by interpreting 13 byte random values as (srcip 4b, dstip 4b, srcport 2b, dstport 2b, prot 1b) tuple. |


### Running Simulation

```
./scripts/makerun.sh -s "<traceFile>"
```
Currently only supports `.bin` generated from distribution.

**Example:**
`./scripts/makerun.sh -s trace/ip5_zipf_1_0.800000_100.000000_100.bin`

**Future Note:**
Format will become the following
```
./scripts/makerun.sh -s "<traceFile>:<topology>:<sketch>"
```
Will support real trace files.

## Configurations Files

| File | Variable | Definitions |
| ------ | ------ | ------ |
| debug_util.hpp | `DebugType` | Define unique type for debugging each module |
| debug_util.hpp | `fileMappings` | Logging file location for each debug type |
| debug_util.hpp | `activeDebugTypes` | Toggle logging and debugging code |
| Seeds.hpp | **...** | Seeds for hash functions and random values. Some seeds must be constant. Distribution seeds are passed as cli args. |
| BloomFilter.hpp | `FLOWSIZE` | The approximate size of the flow passing over the sketch. Used to optimize number of hash functions. |


## License

MIT
