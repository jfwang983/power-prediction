#!/bin/bash
REPO_DIR=$PWD
BAREMETAL_BIN_DIR=$REPO_DIR/power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/build/bareMetalC

ctrl_c() {
     exit 1
}

trap ctrl_c INT

# Custom Chipyard Setup
conda install -n base conda-lock=1.4
git clone https://github.com/jfwang983/power-mappings-chipyard.git
cd power-mappings-chipyard
git checkout latest-version
./build-setup.sh riscv-tools -s 4 -s 6 -s 7 -s 8 -s 9 -f
source ../../miniconda3/etc/profile.d/conda.sh
source env.sh
source /ecad/tools/vlsi.bashrc

# Hammer Setup
cd ../hammer
pip install -e .

# Gemmini Setup
cd ../power-mappings-chipyard/generators/gemmini
git config remote.origin.fetch "+refs/heads/*:refs/remotes/origin/*"
git fetch && git checkout v0.7.1
git submodule update --init --recursive
make -C software/libgemmini install

# Gemmini Config Setup
cd $REPO_DIR
cp -f scripts/templates/CustomConfigs.scala ../power-mappings-chipyard/generators/gemmini/src/main/scala/gemmini/CustomConfigs.scala
cp -f scripts/templates/CustomSoCConfigs.scala ../power-mappings-chipyard/generators/gemmini/src/main/scala/gemmini/CustomSoCConfigs.scala
cd power-mappings-chipyard/generators/gemmini
bash scripts/setup-paths.sh 

# Gemmini-Rocc-Test Setup
cd software/gemmini-rocc-tests
pip install -e .

# Microbenchmark Setup
cd $REPO_DIR
cp -R scripts/templates/bareMetalC/. ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/bareMetalC
cd power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests
bash build.sh

# RTL Setup
cd $REPO_DIR
cd scripts
python modify_mk.py $BAREMETAL_BIN_DIR/mvin_cache_hit_microbenchmark0-baremetal
python modify_yml.py mvin_cache_hit_microbenchmark0
cd ../power-mappings-chipyard/vlsi
make sim-rtl-debug BINARY=$BAREMETAL_BIN_DIR/mvin_cache_hit_microbenchmark0-baremetal LOADMEM=$BAREMETAL_BIN_DIR/mvin_cache_hit_microbenchmark0-baremetal &
wait
make power-rtl