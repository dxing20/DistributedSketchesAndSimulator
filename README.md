Run the container.sh to start dev env.
`bash scripts/container.sh`
Run makerun.sh to build and run main.cpp.


To generate 5-tuple trace from zipf:
`./scripts/makerun.sh -g "zipf:1:0.8:10000:10000"`

To run simulator:
`./scripts/makerun.sh -s trace/ip5_zipf_1_0.800000_100.000000_100.bin`

change logging settings in debug_util. 