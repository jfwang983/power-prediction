#!/bin/bash
SCRIPT_DIR=$PWD

ctrl_c() {
     exit 1
}

trap ctrl_c INT

run_spike() {
     cd $SCRIPT_DIR
     cd ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests
     spike --extension=gemmini build/bareMetalC/$1_spike-baremetal > $SCRIPT_DIR/../data/spike_output/$1_spike.log
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
source .conda-env/etc/profile.d/conda.sh
source env.sh

# Workload Setup
cd $SCRIPT_DIR
python generate_random_matrices.py
python generate_spike_c_files.py
cp -R templates/bareMetalC/. ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/bareMetalC

# Build Binaries
cd ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests
bash build.sh

microbenchmarks=(
    mvin_microbenchmark_basic
    mvout_microbenchmark_basic
    preload_and_compute_microbenchmark_basic
    compute_accumulated_microbenchmark_basic
    mvin_microbenchmark_basic_1_col
    mvin_microbenchmark_basic_2_col
    mvin_microbenchmark_basic_4_col
    mvin_microbenchmark_basic_8_col
    mvin_microbenchmark_basic_16_col
    mvin_microbenchmark_basic_32_col
    mvin_microbenchmark_basic_64_col
    mvin_microbenchmark_mult_addr_basic_1_col
    mvin_microbenchmark_mult_addr_basic_2_col
    mvin_microbenchmark_mult_addr_basic_4_col
    mvin_microbenchmark_mult_addr_basic_8_col
    mvin_microbenchmark_mult_addr_basic_16_col
    mvin_microbenchmark_mult_addr_basic_32_col
    mvin_microbenchmark_mult_addr_basic_64_col
    mvout_microbenchmark_basic_1_col
    mvout_microbenchmark_basic_2_col
    mvout_microbenchmark_basic_4_col
    mvout_microbenchmark_basic_8_col
    mvout_microbenchmark_basic_16_col
    mvout_microbenchmark_basic_32_col
    mvout_microbenchmark_basic_64_col
    mvout_microbenchmark_mult_addr_basic_1_col
    mvout_microbenchmark_mult_addr_basic_2_col
    mvout_microbenchmark_mult_addr_basic_4_col
    mvout_microbenchmark_mult_addr_basic_8_col
    mvout_microbenchmark_mult_addr_basic_16_col
    mvout_microbenchmark_mult_addr_basic_32_col
    mvout_microbenchmark_mult_addr_basic_64_col
    preload_and_compute_microbenchmark_basic_B_1_col
    preload_and_compute_microbenchmark_basic_B_2_col
    preload_and_compute_microbenchmark_basic_B_4_col
    preload_and_compute_microbenchmark_basic_B_8_col
    preload_and_compute_microbenchmark_basic_B_16_col
    preload_and_compute_microbenchmark_basic_A_1_row
    preload_and_compute_microbenchmark_basic_A_2_row
    preload_and_compute_microbenchmark_basic_A_4_row
    preload_and_compute_microbenchmark_basic_A_8_row
    preload_and_compute_microbenchmark_basic_A_16_row
    preload_and_compute_microbenchmark_basic_1_outer
    preload_and_compute_microbenchmark_basic_1_2_outer
    preload_and_compute_microbenchmark_basic_1_4_outer
    preload_and_compute_microbenchmark_basic_1_8_outer
    preload_and_compute_microbenchmark_basic_2_1_outer
    preload_and_compute_microbenchmark_basic_2_outer
    preload_and_compute_microbenchmark_basic_2_4_outer
    preload_and_compute_microbenchmark_basic_2_8_outer
    preload_and_compute_microbenchmark_basic_4_1_outer
    preload_and_compute_microbenchmark_basic_4_2_outer
    preload_and_compute_microbenchmark_basic_4_outer
    preload_and_compute_microbenchmark_basic_4_8_outer
    preload_and_compute_microbenchmark_basic_8_1_outer
    preload_and_compute_microbenchmark_basic_8_2_outer
    preload_and_compute_microbenchmark_basic_8_4_outer
    preload_and_compute_microbenchmark_basic_8_outer
    preload_and_compute_microbenchmark_basic_16_outer
    preload_and_compute_microbenchmark_basic_1_inner
    preload_and_compute_microbenchmark_basic_2_inner
    preload_and_compute_microbenchmark_basic_4_inner
    preload_and_compute_microbenchmark_basic_8_inner
    preload_and_compute_microbenchmark_basic_16_inner
    preload_and_compute_microbenchmark_10_iter_basic
    preload_and_compute_microbenchmark_100_iter_basic
    preload_and_compute_microbenchmark_1000_iter_basic
    preload_and_compute_microbenchmark_addr_acc_basic
    preload_and_compute_microbenchmark_addr_overwrite_basic
    compute_accumulated_microbenchmark_basic_B_1_col
    compute_accumulated_microbenchmark_basic_B_2_col
    compute_accumulated_microbenchmark_basic_B_4_col
    compute_accumulated_microbenchmark_basic_B_8_col
    compute_accumulated_microbenchmark_basic_B_16_col
    compute_accumulated_microbenchmark_basic_A_1_row
    compute_accumulated_microbenchmark_basic_A_2_row
    compute_accumulated_microbenchmark_basic_A_4_row
    compute_accumulated_microbenchmark_basic_A_8_row
    compute_accumulated_microbenchmark_basic_A_16_row
    compute_accumulated_microbenchmark_basic_1_outer
    compute_accumulated_microbenchmark_basic_1_2_outer
    compute_accumulated_microbenchmark_basic_1_4_outer
    compute_accumulated_microbenchmark_basic_1_8_outer
    compute_accumulated_microbenchmark_basic_2_1_outer
    compute_accumulated_microbenchmark_basic_2_outer
    compute_accumulated_microbenchmark_basic_2_4_outer
    compute_accumulated_microbenchmark_basic_2_8_outer
    compute_accumulated_microbenchmark_basic_4_1_outer
    compute_accumulated_microbenchmark_basic_4_2_outer
    compute_accumulated_microbenchmark_basic_4_outer
    compute_accumulated_microbenchmark_basic_4_8_outer
    compute_accumulated_microbenchmark_basic_8_1_outer
    compute_accumulated_microbenchmark_basic_8_2_outer
    compute_accumulated_microbenchmark_basic_8_4_outer
    compute_accumulated_microbenchmark_basic_8_outer
    compute_accumulated_microbenchmark_basic_16_outer
    compute_accumulated_microbenchmark_basic_1_inner
    compute_accumulated_microbenchmark_basic_2_inner
    compute_accumulated_microbenchmark_basic_4_inner
    compute_accumulated_microbenchmark_basic_8_inner
    compute_accumulated_microbenchmark_basic_16_inner
)

workloads=(
    matmul_1
    matmul_2
    matmul_3
    matmul_4
    matmul_5
    matmul_6
    matmul_7
    matmul_8
    matmul_9
    matmul_10
    mlp_1
    mlp_2
    mlp_3
    mlp_4
    mlp_5
    mlp_6
    conv_1
    conv_2
    conv_3
    conv_4
    conv_5
    conv_6
    matmul_1_dense
    matmul_2_dense
    matmul_3_dense
    matmul_4_dense
    matmul_5_dense
    matmul_6_dense
    matmul_7_dense
    matmul_8_dense
    matmul_9_dense
    matmul_10_dense
    mlp_1_dense
    mlp_2_dense
    mlp_3_dense
    mlp_4_dense
    mlp_5_dense
    mlp_6_dense
    conv_1_dense
    conv_2_dense
    conv_3_dense
    conv_4_dense
    conv_5_dense
    conv_6_dense
)

# Microbenchmarks
# Instruction Count Generation
for microbenchmark in "${microbenchmarks[@]}"; do
    run_spike "$microbenchmark"
done

# Waveform Generation
for microbenchmark in "${microbenchmarks[@]}"; do
    run_vcs "$microbenchmark"
done

# Joules Execution
for microbenchmark in "${microbenchmarks[@]}"; do
    run_joules "$microbenchmark"
done

# Workloads
# Instruction Count Generation
for workload in "${workloads[@]}"; do
    run_spike "$workload"
done

# Waveform Generation
for workload in "${workloads[@]}"; do
    run_vcs "$workload"
done

# Joules Execution
for workload in "${workloads[@]}"; do
    run_joules "$workload"
done