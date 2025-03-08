import os
import re

workload_dir = "templates/bareMetalC"
workload_list = [workload for workload in os.listdir(workload_dir) if re.match(r'^.*\.c$', workload) and "microbenchmark" not in workload and workload != 'simple.c' and not workload.endswith("_spike.c")]

for workload in workload_list:
    with open(f"{workload_dir}/{workload}", "r") as file:
        lines = file.readlines()

        modified_lines = []
        for line in lines:
            if '#include "matmul_funcs.h"' in line:
                modified_lines.append(line.replace("matmul_funcs.h", "matmul_funcs_spike.h"))
            elif line.strip().startswith("// printf"):
                modified_lines.append(line.replace("// ", ""))
            else:
                modified_lines.append(line)
        
        with open(f"{workload_dir}/{workload[:-2]}_spike.c", "w") as file:
            file.writelines(modified_lines)