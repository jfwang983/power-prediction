#!/bin/bash
ctrl_c() {
     exit 1
}

trap ctrl_c INT

SCRIPT_DIR=$PWD

WORKLOAD=$1
REDO_FLAG=$2
LOADMEM_FLAG=$3
OPERATION=$4
TYPE=$5
INIT_RTL=$6

# Create CSV for list of binaries to generate
if [ "$OPERATION" = "-regen" ]; then
    cd ../dosa
    python run.py --arch_name gemmini --arch_file dataset/hw/gemmini/arch/arch.yaml --num_mappings 10000 -wl $WORKLOAD
    cd $SCRIPT_DIR
    python sample_extract.py $WORKLOAD -regen
else
    python sample_extract.py $WORKLOAD -append
fi

# Setup Chipyard
cd ../power-mappings-chipyard
source ../../miniconda3/etc/profile.d/conda.sh
source env.sh
source /ecad/tools/vlsi.bashrc

if [ "$OPERATION" = "-append" ]; then
    cd generators/gemmini/software/gemmini-rocc-tests/build/bareMetalC
    NUM_FILES=$(ls -1 | wc -l)
    START=$(($NUM_FILES/3))
    END=$(($START+100))
    cd $SCRIPT_DIR/../power-mappings-chipyard
else
    START=0
    END=100
fi

# Generate binaries
cd generators/gemmini/software/gemmini-rocc-tests/gemmini-data-collection
data_output="./mappings/${WORKLOAD}_random.csv"
./build_script.sh $data_output

cd $SCRIPT_DIR
cd ../power-mappings-chipyard/vlsi

if [ "$INIT_RTL" = "-init" ]; then
    echo "Binary 0"
    binary="${SCRIPT_DIR}/../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/build/bareMetalC/${TYPE}_tilings_0-baremetal"
    make sim-rtl-debug BINARY=$binary LOADMEM=$binary
    make power-rtl
    START=1
fi

# Run RTL Sim
for (( i=$START; i<$END; i++ ))
do
    echo "Binary $i"
    binary="${SCRIPT_DIR}/../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/build/bareMetalC/${TYPE}_tilings_${i}-baremetal"
    ./run_rtl_sim.sh $binary $REDO_FLAG $LOADMEM_FLAG &
    wait
done