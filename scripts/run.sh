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

WORKLOAD=$1
OPERATION=$2
TYPE=$3
INIT_RTL=$4

./create_and_simulate_binaries.sh $WORKLOAD -r -l $OPERATION $TYPE $INIT_RTL

if [ "$OPERATION" = "-append" ]; then
    cd ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/build/bareMetalC/
    NUM_FILES=$(ls -1 | wc -l)
    END=$(($NUM_FILES/3))
    START=$(($END-100))
elif [ "$INIT_RTL" = "-init" ]; then
    START=1
    END=100
else
    START=0
    END=100
fi

cd $SCRIPT_DIR
cd ../power-mappings-chipyard
source ../../miniconda3/etc/profile.d/conda.sh
source env.sh
cd vlsi
cd ../power-mappings-chipyard/vlsi
python run_joules.py $START $END