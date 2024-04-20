import os

validity_benchmark_dir = "templates/validity_benchmarks"
benchmark_list = [benchmark for benchmark in os.listdir(validity_benchmark_dir) if not benchmark.endswith("-spike.c")]
makefile_list = "  "

for benchmark in benchmark_list:
    if benchmark[-11:] == "benchmark.c":
        with open(f"{validity_benchmark_dir}/{benchmark}", "r") as file:
            lines = file.readlines()
        
        modified_lines = []
        for line in lines:
            if line.strip().startswith("// printf"):
                modified_lines.append(line.replace("// ", ""))
            else:
                modified_lines.append(line)
        
        with open(f"{validity_benchmark_dir}/{benchmark[:-2]}-spike.c", "w") as file:
            file.writelines(modified_lines)