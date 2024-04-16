import sys
import yaml

try:     
    sys.argv[1]
except Exception as e:
    raise ValueError("Not enough args")

binary_path = sys.argv[1]

mk_file = "../power-mappings-chipyard/vlsi/custom.mk"

with open(mk_file, 'r') as file:
    lines = file.readlines()

    for i, line in enumerate(lines):
        if line.startswith('BINARY'):
            lines[i] = f'BINARY\t\t\t\t?= {binary_path}\n'
            break

with open(mk_file, 'w') as file:
    file.writelines(lines)
