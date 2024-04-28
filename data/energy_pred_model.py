import re
import pandas as pd

microbenchmark_iterations = 1000
frequency = 31.25 * (10 ** 6)

microbenchmarks = {
    "mvin_cache_hit_microbenchmark_random": {"start_time": 219216.0, "end_time": 779216.0},
    "mvout_microbenchmark_random": {"start_time": 270416.0, "end_time": 1076816.0},
    "preload_and_compute_random_test2": {"start_time": 417616.0, "end_time": 1435216.0}
}

validity_benchmarks = {
    "tiled_matmul_ws_random_benchmark": {"start_time": 7041616.0, "end_time": 9963216.0},
    "mlp_random_benchmark": {"start_time": 35240020.0, "end_time": 36756820.0}
}

# Benchmark Tables
validity_benchmark_data_table = {}
microbenchmark_data_table = {}

def create_microbenchmark_table():
    global microbenchmark_data_table
    microbenchmark_data_table = {}
    for microbenchmark in microbenchmarks.keys():
        extracted_data = extract_data(microbenchmarks, microbenchmark)
        table_data = {
            "setup_cycle_count": extracted_data["setup_cycle_count"],
            "gemmini_setup_cycle_count": extracted_data["gemmini_setup_cycle_count"],
            "cycle_count": extracted_data["cycle_count"],
            "dynamic_power_breakdown": extracted_data["dynamic_power"],
            "dynamic_energy_breakdown": extracted_data["dynamic_energy"],
            "epi_breakdown": {key: extracted_data["dynamic_energy"][key]/microbenchmark_iterations for key in extracted_data["dynamic_energy"].keys()},
            "epi": sum(extracted_data["dynamic_energy"].values())/microbenchmark_iterations
        }
        microbenchmark_data_table[microbenchmark] = table_data

def create_validity_benchmark_table():
    global validity_benchmark_data_table
    validity_benchmark_data_table = {}
    for benchmark in validity_benchmarks.keys():
        extracted_data = extract_data(validity_benchmarks, benchmark)
        inst_count = get_inst_breakdown(benchmark)
        table_data = {
            "setup_cycle_count": extracted_data["setup_cycle_count"],
            "gemmini_setup_cycle_count": extracted_data["gemmini_setup_cycle_count"],
            "cycle_count": extracted_data["cycle_count"],
            "inst_count": inst_count,
            "dynamic_power_breakdown": extracted_data["dynamic_power"],
            "dynamic_energy_breakdown": extracted_data["dynamic_energy"],
            "dynamic_energy": sum(extracted_data["dynamic_energy"].values())
        }
        validity_benchmark_data_table[benchmark] = table_data


def extract_data(table, workload):
    report_name = f"{workload}-baremetal"

    # Cycle Count Extraction
    sim_report_path = f"vcs_output/{report_name}.log"
    phrase_pattern_1 = r"Setup cycles taken: (\d+)"
    phrase_pattern_2 = r"Gemmini Setup cycles taken: (\d+)"
    phrase_pattern_3 = r"Cycles taken: (\d+)"
    with open(sim_report_path, 'r') as file:
        file_content = file.read()
        match_1 = re.search(phrase_pattern_1, file_content)
        match_2 = re.search(phrase_pattern_2, file_content)
        match_3 = re.search(phrase_pattern_3, file_content)
        setup_cycle_count = int(match_1.group(1))
        cycle_count = int(match_3.group(1))
        try:
            gemmini_setup_cycle_count = int(match_2.group(1))
        except Exception:
            gemmini_setup_cycle_count = 0
    
    # Dynamic Power Extraction
    dynamic_power = {}
    modules = ["spad", "acc", "mesh"]
    for module in modules:
        df = pd.read_csv(f"joules_output/{workload}/{module}_power_plot.csv")
        sim_times = df['sim_time'].tolist()
        power = df['power'].tolist()
        
        start_index = sim_times.index(table[workload]["start_time"])
        end_index  = sim_times.index(table[workload]["end_time"])
        num_rows = end_index - start_index + 1

        dynamic_power[module] = 0
        for i in range(start_index, end_index + 1):
            dynamic_power[module] += power[i]
        dynamic_power[module] /= num_rows
    
    # Dynamic Energy Computation
    spad_energy = dynamic_power["spad"] * (10 ** 3) * (cycle_count/frequency)
    acc_energy = dynamic_power["acc"] * (10 ** 3) * (cycle_count/frequency)
    mesh_energy = dynamic_power["mesh"] * (10 ** 3) * (cycle_count/frequency)

    extracted_data = {
        "setup_cycle_count": setup_cycle_count,
        "gemmini_setup_cycle_count": gemmini_setup_cycle_count,
        "cycle_count": cycle_count,
        "dynamic_power": dynamic_power,
        "dynamic_energy": {"spad": spad_energy, "acc": acc_energy, "mesh": mesh_energy}
    }
    return extracted_data

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


def epi_model(inst_count):
    energy_breakdown = []
    for module in ["spad", "acc", "mesh"]:
        mvin_cache_hit_energy = microbenchmark_data_table["mvin_cache_hit_microbenchmark_random"]["epi_breakdown"][module] * inst_count["mvin_cache_hit"]
        mvout_energy = microbenchmark_data_table["mvout_microbenchmark_random"]["epi_breakdown"][module] * inst_count["mvout"]
        compute_energy = microbenchmark_data_table["preload_and_compute_random_test2"]["epi_breakdown"][module] * inst_count["compute"]
        energy_breakdown.append(mvin_cache_hit_energy + mvout_energy + compute_energy)
    return energy_breakdown


create_microbenchmark_table()
create_validity_benchmark_table()

print("***EPI Table***")
for microbenchmark in microbenchmark_data_table.keys():
    print(microbenchmark)
    print(microbenchmark_data_table[microbenchmark], "\n")

print("\n")

print("***Validation Results***")
for benchmark in validity_benchmark_data_table.keys():
    predicted_energy = epi_model(validity_benchmark_data_table[benchmark]["inst_count"])
    actual_energy_breakdown = validity_benchmark_data_table[benchmark]["dynamic_energy_breakdown"]
    actual_energy = sum([actual_energy_breakdown[key] for key in actual_energy_breakdown.keys()])
    percent_error = abs((actual_energy - sum(predicted_energy))/sum(predicted_energy)) * 100

    print(benchmark)
    print("inst counts")
    print(validity_benchmark_data_table[benchmark]["inst_count"], "\n")
    print("energy breakdown")
    print(validity_benchmark_data_table[benchmark]["dynamic_energy_breakdown"], "\n")
    print("Actual Energy:", actual_energy, "uJ")
    print("Predicted Energy Breakdown:", predicted_energy)
    print("Predicted Energy:", sum(predicted_energy), "uJ")
    print(f"Percent Error: {round(percent_error, 2)}%\n\n")
