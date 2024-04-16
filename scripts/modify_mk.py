import sys
import os

try:     
    sys.argv[1]
except Exception as e:
    raise ValueError("Not enough args")

workload = sys.argv[1]

mk_file = "../power-mappings-chipyard/vlsi/custom.mk"
binary_path = f"{os.getcwd()}/../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/build/bareMetalC/{workload}-baremetal"

with open(mk_file, 'r') as file:
    lines = file.readlines()

    for i, line in enumerate(lines):
        if line.startswith('BINARY'):
            lines[i] = f'BINARY\t\t\t\t?= {binary_path}\n'
            break

with open(mk_file, 'w') as file:
    file.writelines(lines)
