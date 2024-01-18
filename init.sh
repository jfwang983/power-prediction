#!/bin/bash
REPO_DIR=$PWD
# DOSA and Timeloop Setup
cd dosa/accelergy-timeloop-infrastructure/src/accelergy
pip install .
cd ../cacti
make
cd ../accelergy-cacti-plug-in
pip install .
cd ../accelergy-aladdin-plug-in
pip install .
cd ../accelergy-table-based-plug-ins
pip install .
cd ../timeloop/src
ln -s ../pat-public/src/pat .
cd ..
scons --accelergy -j4 --static
cd ../../..
pip install -e .
cd ..

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
./gemmini_config_init.sh
cd ../power-mappings-chipyard/generators/gemmini
./scripts/setup-paths.sh 

# Gemmini-Rocc-Test Setup
cd software/gemmini-rocc-tests
git checkout power
pip install -e .
./build.sh