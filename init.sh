#!/bin/bash
REPO_DIR=$PWD

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
cd scripts
bash gemmini_config_init.sh
cd ../power-mappings-chipyard/generators/gemmini
bash scripts/setup-paths.sh 

# Gemmini-Rocc-Test Setup
cd software/gemmini-rocc-tests
pip install -e .

# Microbenchmark Setup
cd $REPO_DIR
cd scripts
bash gemmini_microbenchmark_init.sh
cd ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests
bash build.sh

# RTL Setup
cd $REPO_DIR
cd scripts
bash init_rtl.sh