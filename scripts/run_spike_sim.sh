#!/bin/bash
SCRIPT_DIR=$PWD
WORKLOAD=$1

# Setup Tools
cd ../power-mappings-chipyard
source ../../miniforge3/etc/profile.d/conda.sh
source env.sh

# Binary Setup
cd $SCRIPT_DIR
python generate_random_matrices.py
cp -R templates/bareMetalC/. ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/bareMetalC

# Build Binaries
cd ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests
bash build.sh

cd $SCRIPT_DIR
cd ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests
spike --extension=gemmini build/bareMetalC/$WORKLOAD-baremetal