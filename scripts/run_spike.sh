#!/bin/bash
SCRIPT_DIR=$PWD

# Setup Tools
cd ../power-mappings-chipyard
source ../../miniconda3/etc/profile.d/conda.sh
source env.sh
source /ecad/tools/vlsi.bashrc

# Binary Setup
cd $SCRIPT_DIR
cp -R templates/bareMetalC/. ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/bareMetalC

# Build Binaries
cd ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests
bash build.sh

cd $SCRIPT_DIR
cd ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests
spike --extension=gemmini build/bareMetalC/$1-baremetal