#!/bin/bash
cd power-mappings-chipyard
source ../../miniconda3/etc/profile.d/conda.sh
source env.sh
source /ecad/tools/vlsi.bashrc

cd vlsi
make sim-rtl-debug
make power-rtl
