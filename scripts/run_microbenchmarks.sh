#!/bin/bash
SCRIPT_DIR=$PWD

ctrl_c() {
     exit 1
}

trap ctrl_c INT

run_vcs() {
     cd $SCRIPT_DIR
     python modify_mk.py $1
     cd ../power-mappings-chipyard/vlsi/
     make redo-sim-rtl-debug args="--only_step run_simulation" > vcs_output/$1-baremetal.log &
     wait
     echo "Finished RTL Simulation for ${1}"
}

run_joules() {
     cd $SCRIPT_DIR
     python modify_mk.py $1
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
mkdir -p vcs_output

# Waveform Generation
# mvin microbenchmarks
run_vcs mvin_cache_hit_microbenchmark0
run_vcs mvin_cache_hit_microbenchmark1
run_vcs mvin_cache_hit_microbenchmark2
run_vcs mvin_cache_hit_microbenchmark3
run_vcs mvin_cache_hit_microbenchmark4

# mvout microbenchmarks
run_vcs mvout_microbenchmark0
run_vcs mvout_microbenchmark1
run_vcs mvout_microbenchmark2
run_vcs mvout_microbenchmark3
run_vcs mvout_microbenchmark4

# preload_and_compute microbenchmarks
run_vcs preload_and_compute_0
run_vcs preload_and_compute_1
run_vcs preload_and_compute_random

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