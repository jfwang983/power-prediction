#!/bin/bash
SCRIPT_DIR=$PWD

ctrl_c() {
     exit 1
}

trap ctrl_c INT

if [ $# -eq 0 ]; then
    echo "not enough args"
    exit 1
fi

WORKLOAD=$1
JOULES_REPORT_NAME=$2

# Setup Tools
cd ../power-mappings-chipyard
source ../../miniconda3/etc/profile.d/conda.sh
source env.sh
source /ecad/tools/vlsi.bashrc

# Microbenchmark Setup
cd $SCRIPT_DIR
cp -R templates/bareMetalC/. ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/bareMetalC

# Build Binaries
cd ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests
bash build.sh

cd $SCRIPT_DIR
cd ../data
mkdir -p vcs_output
mkdir -p joules_output

# RTL Setup
cd $SCRIPT_DIR
python modify_mk.py $WORKLOAD
python modify_yml.py $WORKLOAD $JOULES_REPORT_NAME
cd ../power-mappings-chipyard/vlsi
make redo-sim-rtl-debug args="--only_step run_simulation" > $SCRIPT_DIR/../data/vcs_output/$WORKLOAD-baremetal.log &
wait
echo "Finished RTL Simulation for ${WORKLOAD}"
make redo-power-rtl args="--only_step report_power" &
wait
mkdir -p ../../data/joules_output/$WORKLOAD
cp -R build/chipyard.harness.TestHarness.CustomGemminiSoCConfig-ChipTop/power-rtl-rundir/reports/$WORKLOAD-baremetal-gemmini.power.rpt $SCRIPT_DIR/../data/joules_output/$WORKLOAD/$WORKLOAD-baremetal-gemmini.power.rpt
cp -R build/chipyard.harness.TestHarness.CustomGemminiSoCConfig-ChipTop/power-rtl-rundir/reports/$WORKLOAD-baremetal-gemmini.hier.power.rpt $SCRIPT_DIR/../data/joules_output/$WORKLOAD/$WORKLOAD-baremetal-gemmini.hier.power.rpt
cd $SCRIPT_DIR
python compile_power_plots.py $WORKLOAD