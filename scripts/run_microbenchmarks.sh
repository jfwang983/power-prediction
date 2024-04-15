#!/bin/bash
SCRIPT_DIR=$PWD
BAREMETAL_BIN_DIR=$SCRIPT_DIR/../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/build/bareMetalC

ctrl_c() {
     exit 1
}

trap ctrl_c INT

run_vcs() {
     make redo-sim-rtl-debug BINARY=$1 LOADMEM=$1 &
     wait
}

run_joules() {
     cd ../../scripts
     python modify_yml.py $1
     cd ../power-mappings-chipyard/vlsi/
     make redo-power-rtl args="--only_step report_power" &
     wait
}

# Setup Tools
cd ../power-mappings-chipyard
source ../../miniconda3/etc/profile.d/conda.sh
source env.sh
source /ecad/tools/vlsi.bashrc

# Build Binaries
cd generators/gemmini/software/gemmini-rocc-tests
bash build.sh

cd ../../../../vlsi

# Waveform Generation
# mvin microbenchmarks
run_vcs $BAREMETAL_BIN_DIR/mvin_cache_hit_microbenchmark0-baremetal
run_vcs $BAREMETAL_BIN_DIR/mvin_cache_hit_microbenchmark1-baremetal
run_vcs $BAREMETAL_BIN_DIR/mvin_cache_hit_microbenchmark2-baremetal
run_vcs $BAREMETAL_BIN_DIR/mvin_cache_hit_microbenchmark3-baremetal
run_vcs $BAREMETAL_BIN_DIR/mvin_cache_hit_microbenchmark4-baremetal

# mvout microbenchmarks
run_vcs $BAREMETAL_BIN_DIR/mvout_microbenchmark0-baremetal
run_vcs $BAREMETAL_BIN_DIR/mvout_microbenchmark1-baremetal
run_vcs $BAREMETAL_BIN_DIR/mvout_microbenchmark2-baremetal
run_vcs $BAREMETAL_BIN_DIR/mvout_microbenchmark3-baremetal
run_vcs $BAREMETAL_BIN_DIR/mvout_microbenchmark4-baremetal

# preload_and_compute microbenchmarks
run_vcs $BAREMETAL_BIN_DIR/preload_and_compute_0-baremetal
run_vcs $BAREMETAL_BIN_DIR/preload_and_compute_1-baremetal
run_vcs $BAREMETAL_BIN_DIR/preload_and_compute_random-baremetal

# Joules Execution
# mvin microbenchmarks
run_joules mvin_cache_hit_microbenchmark0
run_joules mvin_cache_hit_microbenchmark1
run_joules mvin_cache_hit_microbenchmark2
run_joules mvin_cache_hit_microbenchmark3
run_joules mvin_cache_hit_microbenchmark4

# mvout microbenchmarks
run_joules mvout_microbenchmark0
run_joules mvout_microbenchmark1
run_joules mvout_microbenchmark2
run_joules mvout_microbenchmark3
run_joules mvout_microbenchmark4

# preload_and_compute microbenchmarks
run_joules preload_and_compute_0
run_joules preload_and_compute_1
run_joules preload_and_compute_random