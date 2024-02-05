#!/bin/bash
ctrl_c() {
     exit 1
}

trap ctrl_c INT

SCRIPT_DIR=$PWD

if [ $# -eq 0 ]; then
    echo "not enough args"
    exit 1
fi

./create_and_simulate_binaries.sh $1 -r -l $2

START=0
END=100
if [ "$2" = "-append" ]; then
    cd ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/build/bareMetalC/
    NUM_FILES=$(ls -1 | wc -l)
    START=$(($NUM_FILES/3))
    END=$(($START+100))
fi

cd $SCRIPT_DIR
cd ../power-mappings-chipyard
source ../../miniconda3/etc/profile.d/conda.sh
source env.sh
cd vlsi
cd ../power-mappings-chipyard/vlsi
python run_joules.py $START $END