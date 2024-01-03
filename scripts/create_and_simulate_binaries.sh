#!/bin/bash
ctrl_c() {
     exit 1
}

trap ctrl_c INT

SCRIPT_DIR=$PWD
cd ../power-mappings-chipyard/vlsi
source /ecad/tools/vlsi.bashrc
cd $SCRIPT_DIR

# Create CSV for list of binaries to generate
if [ "$4" = "-regen" ]; then
    cd ../dosa
    python run.py --arch_name gemmini --arch_file dataset/hw/gemmini/arch/arch.yaml --num_mappings 10000 -wl $1
    cd $SCRIPT_DIR
    python sample_extract.py -regen
else
    python sample_extract.py -append
fi

# Generate binaries
data_output="./mappings/$1_random.csv"
cd ../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/gemmini-data-collection
./build_script.sh $data_output

cd ../build/bareMetalC/
NUM_FILES=$(ls -1 | wc -l)
START=$(($NUM_FILES/3))
END=$(($START+100))

if [ "$4" = "-regen" ]; then
    START=0
    END=$(($NUM_FILES/3))
fi


# Run RTL Sim
cd $SCRIPT_DIR
cd ../power-mappings-chipyard/vlsi
for (( i=$START; i<$END; i++ ))
do
    echo "Binary $i"
    binary="../generators/gemmini/software/gemmini-rocc-tests/build/bareMetalC/matmul_tilings_$i-baremetal"
    ./run_rtl_sim.sh $binary $2 $3 &
    wait
done