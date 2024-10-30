import os
import re

microbenchmark_dir = "templates/bareMetalC"
microbenchmark_list = [microbenchmark for microbenchmark in os.listdir(microbenchmark_dir) if re.match(r'^.*\.c$', microbenchmark) and microbenchmark != 'simple.c' and not microbenchmark.endswith("_spike.c")]
makefile_list = "  "

for microbenchmark in microbenchmark_list:
    with open(f"{microbenchmark_dir}/{microbenchmark}", "r") as file:
        lines = file.readlines()

        modified_lines = []
        for line in lines:
            if '#include "matmul_funcs.h"' in line:
                modified_lines.append(line.replace("matmul_funcs.h", "matmul_funcs_spike.h"))
            elif line.strip().startswith("// printf"):
                modified_lines.append(line.replace("// ", ""))
            else:
                modified_lines.append(line)
        
        with open(f"{microbenchmark_dir}/{microbenchmark[:-2]}_spike.c", "w") as file:
            file.writelines(modified_lines)