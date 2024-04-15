#!/bin/bash
SCRIPT_DIR=$PWD
BAREMETAL_BIN_DIR=$SCRIPT_DIR/../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/build/bareMetalC

ctrl_c() {
     exit 1
}

trap ctrl_c INT

python modify_yml.py mvin_cache_hit_microbenchmark0
cd ../power-mappings-chipyard/vlsi/
make sim-rtl-debug BINARY=$BAREMETAL_BIN_DIR/mvin_cache_hit_microbenchmark0-baremetal LOADMEM=$BAREMETAL_BIN_DIR/mvin_cache_hit_microbenchmark0-baremetal &
wait
make power-rtl