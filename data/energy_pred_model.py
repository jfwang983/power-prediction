import os
import re

frequency = 31.25 * (10 ** 6)

workloads = ["matmul_test"]

microbenchmarks = {
    # "mvin_cache_hit_microbenchmark_0": 1000,
    "mvin_cache_hit_microbenchmark_random": 1000,
    "mvin_cache_miss_microbenchmark_random": 1000,
    # "mvout_microbenchmark_0": 1000,
    "mvout_microbenchmark_random": 1000,
    "preload_and_compute_sparse_100": 1000,
    "preload_and_compute_sparse_60": 1000,
    "preload_and_compute_sparse_20": 1000,
    "preload_and_compute_sparse_0": 1000,
    "preload_and_compute_random": 1000
}

validity_benchmarks = {
    "tiled_matmul_ws_benchmark": 100
}

baseline_power = []
benchmark_energy_table = {}
energy_table = {}

def create_benchmark_table():
    global benchmark_energy_table
    benchmark_energy_table = {}
    for key in validity_benchmarks.keys():
        energy_breakdown = get_energy_breakdown(key)
        per_exec_breakdown = [energy/validity_benchmarks[key] for energy in energy_breakdown]
        inst_count = get_inst_breakdown(key)

        benchmark_data = {}
        benchmark_data["energy"] = {"spad": per_exec_breakdown[0], "acc": per_exec_breakdown[1], "mesh": per_exec_breakdown[2]}
        benchmark_data["inst_count"] = inst_count
        benchmark_energy_table[key] = benchmark_data

def create_energy_table():
    global energy_table
    energy_table = {}
    for key in microbenchmarks.keys():
        energy_breakdown = get_energy_breakdown(key)
        energy_table[key] = sum(energy_breakdown) / microbenchmarks[key]


def get_baseline_power():
    spad_power_report_path = "joules_output/inactive_spad-baremetal-gemmini.hier.power.rpt"
    mesh_acc_power_report_path = "joules_output/inactive_mesh_acc-baremetal-gemmini.hier.power.rpt"

    gemmini_hierarchy = "/ChipTop/system/tile_prci_domain/tile_reset_domain_tile/gemmini/"
    gemmini_spad_modules = [gemmini_hierarchy + f"spad/spad_mems_{i}/mem/mem_ext" for i in range(4)]
    gemmini_acc_modules = [gemmini_hierarchy + f"spad/acc_mems_{i}" for i in range(2)]
    gemmini_mesh_module = gemmini_hierarchy + "ex_controller/mesh"

    spad_dynamic_power = 0
    acc_dynamic_power = 0
    mesh_dynamic_power = 0

    # spad
    with open(spad_power_report_path, 'r') as file:
        lines = file.readlines()[5:-1]
    
    for line in lines:
        columns = line.split()
        internal_power = convert_scientific_notation(columns[3])
        switching_power = convert_scientific_notation(columns[4])
        dynamic_power = internal_power + switching_power
        if columns[7] in gemmini_spad_modules:
            spad_dynamic_power += dynamic_power

    # mesh and acc
    with open(mesh_acc_power_report_path, 'r') as file:
        lines = file.readlines()[5:-1]

    for line in lines:
        columns = line.split()
        internal_power = convert_scientific_notation(columns[3])
        switching_power = convert_scientific_notation(columns[4])
        dynamic_power = internal_power + switching_power
        if columns[7] in gemmini_acc_modules:
            acc_dynamic_power += dynamic_power
        elif columns[7] == gemmini_mesh_module:
            mesh_dynamic_power += dynamic_power
    
    return [spad_dynamic_power, acc_dynamic_power, mesh_dynamic_power]

def get_inst_breakdown(workload):
    inst_count = {
        'mvin_cache_hit': 0,
        'mvin_cache_miss': 0,
        'mvout': 0,
        'compute': 0
    }

    spike_output = f"spike_output/{workload}-spike_output.log"
    with open(spike_output, 'r') as file:
        for line in file:
            if "mvin" in line:
                inst_count["mvin_cache_hit"] += 1
            elif "mvout" in line:
                inst_count["mvout"] += 1
            elif "compute" in line:
                inst_count["compute"] += 1

    return inst_count

def get_energy_breakdown(workload):
    report_name = f"{workload}-baremetal"

    # Cycle Count Extraction
    sim_report_path = f"vcs_output/{report_name}.log"
    with open(sim_report_path, 'r') as file:
        file_content = file.read()
        phrase_pattern = r"Cycles taken: (\d+)"
        match = re.search(phrase_pattern, file_content)
        cycle_count = int(match.group(1))

    # Dynamic Power Extraction
    power_report_path = f"joules_output/{report_name}-gemmini.hier.power.rpt"

    gemmini_hierarchy = "/ChipTop/system/tile_prci_domain/tile_reset_domain_tile/gemmini/"
    gemmini_spad_modules = [gemmini_hierarchy + f"spad/spad_mems_{i}/mem/mem_ext" for i in range(4)]
    gemmini_acc_modules = [gemmini_hierarchy + f"spad/acc_mems_{i}" for i in range(2)]
    gemmini_mesh_module = gemmini_hierarchy + "ex_controller/mesh"

    spad_dynamic_power = 0
    acc_dynamic_power = 0
    mesh_dynamic_power = 0

    with open(power_report_path, 'r') as file:
        lines = file.readlines()[5:-1]

    for line in lines:
        columns = line.split()
        internal_power = convert_scientific_notation(columns[3])
        switching_power = convert_scientific_notation(columns[4])
        dynamic_power = internal_power + switching_power
        if columns[7] in gemmini_spad_modules:
            spad_dynamic_power += dynamic_power
        elif columns[7] in gemmini_acc_modules:
            acc_dynamic_power += dynamic_power
        elif columns[7] == gemmini_mesh_module:
            mesh_dynamic_power += dynamic_power

    spad_energy = (spad_dynamic_power - baseline_power[0]) * (10 ** 3) * (cycle_count/frequency)
    acc_energy = (acc_dynamic_power - baseline_power[1]) * (10 ** 3) * (cycle_count/frequency)
    mesh_energy = (mesh_dynamic_power - baseline_power[2]) * (10 ** 3) * (cycle_count/frequency)

    return [spad_energy, acc_energy, mesh_energy]

def convert_scientific_notation(value):
    try:
        return float(value)
    except ValueError:
        return value


def epi_model(inst_count):
    mvin_cache_hit_energy = energy_table["mvin_cache_hit_microbenchmark_random"] * inst_count["mvin_cache_hit"]
    mvout_energy = energy_table["mvout_microbenchmark_random"] * inst_count["mvout"]
    compute_energy = energy_table["preload_and_compute_random"] * inst_count["compute"]
    return mvin_cache_hit_energy + mvout_energy + compute_energy


baseline_power = get_baseline_power()
create_benchmark_table()
create_energy_table()

predicted_energy = epi_model(benchmark_energy_table["tiled_matmul_ws_benchmark"]["inst_count"])
actual_energy_breakdown = benchmark_energy_table["tiled_matmul_ws_benchmark"]["energy"]
actual_energy = sum([actual_energy_breakdown[key] for key in actual_energy_breakdown.keys()])

print("tiled_matmul_ws_benchmark")
print(benchmark_energy_table["tiled_matmul_ws_benchmark"]["inst_count"])
print(benchmark_energy_table["tiled_matmul_ws_benchmark"]["energy"])
print("Actual Energy:", actual_energy, "uJ")
print("Predicted Energy:", predicted_energy, "uJ")