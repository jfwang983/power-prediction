#!/bin/bash
SCRIPT_DIR=$PWD

ctrl_c() {
     exit 1
}

trap ctrl_c INT

run_spike() {
     cd $SCRIPT_DIR
     cd ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests
     spike --extension=gemmini build/bareMetalC/$1-spike-baremetal > $SCRIPT_DIR/../data/spike_output/$1-spike_output.log
     echo "Finished Spike Functional Simulation for ${1}"
}

run_vcs() {
     cd $SCRIPT_DIR
     python modify_mk.py $1
     cd ../power-mappings-chipyard/vlsi/
     make redo-sim-rtl-debug args="--only_step run_simulation" > $SCRIPT_DIR/../data/vcs_output/$1-baremetal.log &
     wait
     echo "Finished RTL Simulation for ${1}"
}

run_joules() {
     cd $SCRIPT_DIR
     python modify_mk.py $1
     python modify_yml.py $1 $1
     cd ../power-mappings-chipyard/vlsi/
     make redo-power-rtl args="--only_step report_power" &
     wait
     mkdir -p ../../data/joules_output/$1
     cp -R build/chipyard.harness.TestHarness.CustomGemminiSoCConfig-ChipTop/power-rtl-rundir/reports/$1-baremetal-gemmini.power.rpt $SCRIPT_DIR/../data/joules_output/$1/$1-baremetal-gemmini.power.rpt
     cp -R build/chipyard.harness.TestHarness.CustomGemminiSoCConfig-ChipTop/power-rtl-rundir/reports/$1-baremetal-gemmini.hier.power.rpt $SCRIPT_DIR/../data/joules_output/$1/$1-baremetal-gemmini.hier.power.rpt
     cd $SCRIPT_DIR
     python compile_power_plots.py $1
}

# Setup Tools
cd ../power-mappings-chipyard
source ../../miniconda3/etc/profile.d/conda.sh
source env.sh
source /ecad/tools/vlsi.bashrc

# Microbenchmark Setup
cd $SCRIPT_DIR
python generate_spike_c_files.py
cp -R templates/validity_benchmarks/. ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/bareMetalC

# Build Binaries
cd ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests
bash build.sh

cd $SCRIPT_DIR
cd ../data
mkdir -p spike_output
mkdir -p vcs_output
mkdir -p joules_output

# Instruction Count Generation
run_spike tiled_matmul_ws_random_benchmark
run_spike mlp_random_benchmark

# Waveform Generation
run_vcs tiled_matmul_ws_random_benchmark
run_vcs mlp_random_benchmark

# Joules Execution
run_joules tiled_matmul_ws_random_benchmark
run_joules mlp_random_benchmark