#!/bin/bash
REPO_DIR=$PWD
BAREMETAL_BIN_DIR=$REPO_DIR/power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/build/bareMetalC

ctrl_c() {
     exit 1
}

trap ctrl_c INT

# Repo Setup
git submodule init
git submodule update

# Data Setup
cd $REPO_DIR/data
mkdir -p spike_output
mkdir -p vcs_output
mkdir -p joules_output

# Custom Chipyard Setup
cd $REPO_DIR
conda install -n base conda-lock=1.4
git clone https://github.com/jfwang983/power-mappings-chipyard.git
cd power-mappings-chipyard
git checkout energy-bwrc
./build-setup.sh riscv-tools -s 4 -s 6 -s 7 -s 8 -s 9
source .conda-env/etc/profile.d/conda.sh
source env.sh

# Gemmini Setup
cd ../power-mappings-chipyard/generators/gemmini
git submodule update --init --recursive
make -C software/libgemmini install

# Gemmini Config Setup
cd $REPO_DIR
cp -f scripts/templates/CustomConfigs.scala power-mappings-chipyard/generators/gemmini/src/main/scala/gemmini/CustomConfigs.scala
cp -f scripts/templates/CustomCPUConfigs.scala power-mappings-chipyard/generators/gemmini/src/main/scala/gemmini/CustomCPUConfigs.scala
cp -f scripts/templates/CustomSoCConfigs.scala power-mappings-chipyard/generators/gemmini/src/main/scala/gemmini/CustomSoCConfigs.scala
cd power-mappings-chipyard/generators/gemmini
bash scripts/setup-paths.sh

# Gemmini-Rocc-Test Setup
cd software/gemmini-rocc-tests
pip install -e .

# Microbenchmark Setup
cd $REPO_DIR
cp -R scripts/templates/bareMetalC/. power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/bareMetalC
cd power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests
bash build.sh

# Config Setup
cd $REPO_DIR/scripts
python modify_mk.py simple
python modify_yml.py simple simple

# Hammer Setup
cd ../power-mappings-chipyard/vlsi
echo "Y" | pip uninstall hammer-vlsi
git clone git@github.com:ucb-bar/hammer.git
pip install -e hammer/
pip install -e hammer-intech22-plugin/

# RTL Setup
make sim-rtl-debug > $REPO_DIR/data/vcs_output/simple-baremetal.log &
wait
make power-rtl &
wait

# Data Setup
cd $REPO_DIR/data
mkdir -p joules_output/simple
cp -R build/chipyard.harness.TestHarness.CustomGemminiSoCConfig-ChipTop/power-rtl-rundir/reports/simple-baremetal-gemmini.power.rpt $REPO_DIR/data/joules_output/simple/simple-baremetal-gemmini.power.rpt
cp -R build/chipyard.harness.TestHarness.CustomGemminiSoCConfig-ChipTop/power-rtl-rundir/reports/simple-baremetal-gemmini.hier.power.rpt $REPO_DIR/data/joules_output/simple/simple-baremetal-gemmini.hier.power.rpt
cd $REPO_DIR/scripts
python compile_power_plots.py simple