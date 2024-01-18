#!/bin/bash
ctrl_c() {
     exit 1
}

trap ctrl_c INT

SCRIPT_DIR=$PWD

# Create CSV for list of binaries to generate
if [ "$4" = "-regen" ]; then
    cd ../dosa
    python run.py --arch_name gemmini --arch_file dataset/hw/gemmini/arch/arch.yaml --num_mappings 10000 -wl $1
    cd $SCRIPT_DIR
    python sample_extract.py $1 -regen
else
    python sample_extract.py $1 -append
fi

# Setup Chipyard
cd ../power-mappings-chipyard
source ../../miniconda3/etc/profile.d/conda.sh
source env.sh
source /ecad/tools/vlsi.bashrc

# Generate binaries
data_output="./mappings/$1_random.csv"
cd generators/gemmini/software/gemmini-rocc-tests/gemmini-data-collection
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