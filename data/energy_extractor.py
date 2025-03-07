import os
import re
import pathlib
import pandas as pd

clk_period = 10
frequency = 100 * (10 ** 6)

modules = ["gemmini", "spad", "acc", "mesh"]

data_dir = pathlib.Path(__file__).parent.resolve()

# modes:
# automatic - automates finding ranges
# ranged - specified ranges of plot
# full - entire range of plot (except power bug in setup)
def extract_data(workload, mode="automatic", start_time=None, end_time=None):
    report_name = f"{workload}-baremetal"

    # Cycle Count Extraction
    sim_report_path = f"{data_dir}/vcs_output/{report_name}.log"
    phrase_pattern_1 = r"Setup cycles taken: (\d+)"
    phrase_pattern_2 = r"Cycles taken: (\d+)"
    with open(sim_report_path, 'r') as file:
        file_content = file.read()
        match_1 = re.search(phrase_pattern_1, file_content)
        match_2 = re.search(phrase_pattern_2, file_content)
        setup_cycle_count = int(match_1.group(1)) if match_1 else None
        cycle_count = int(match_2.group(1)) if match_2 else None
 
    # Dynamic Power/Energy Extraction
    if mode == "full":
        dynamic_power, plot_cycle_count = get_average_power_full(workload)
        dynamic_energy = {module : dynamic_power[module] * (10 ** 3) * (plot_cycle_count/frequency) for module in dynamic_power.keys()}
    elif mode == "joules":
        dynamic_power, plot_cycle_count = get_average_power_joules(workload)
        dynamic_energy = {module : dynamic_power[module] * (10 ** 3) * (plot_cycle_count/frequency) for module in dynamic_power.keys()}
    elif mode == "ranged":
        dynamic_power, plot_cycle_count = get_average_power_ranged(workload, start_time, end_time)
        dynamic_energy = {module : dynamic_power[module] * (10 ** 3) * (cycle_count/frequency) if cycle_count else None for module in dynamic_power.keys()}
    else:
        dynamic_power, plot_cycle_count = get_average_power(workload)
        dynamic_energy = {module : dynamic_power[module] * (10 ** 3) * (cycle_count/frequency) if cycle_count else None for module in dynamic_power.keys()}


    extracted_data = {
        "setup_cycle_count": setup_cycle_count,
        "cycle_count": cycle_count,
        "plot_cycle_count": plot_cycle_count,
        "dynamic_power": dynamic_power,
        "dynamic_energy": dynamic_energy
    }
    return extracted_data


def get_average_power(workload):
    dynamic_power = {}
    modules = ["gemmini", "spad", "acc", "mesh"]
    for module in modules:
        df = pd.read_csv(f"{data_dir}/joules_output/{workload}/{module}_power_plot.csv")
        sim_times = df['sim_time'].tolist()
        power_vals = df['power'].tolist()

        # Find Start Index
        start_index = 2
        while start_index < len(power_vals) - 1 and power_vals[start_index] < 0.1:
            start_index += 1

        end_index = len(power_vals) - 1
        while start_index < end_index and power_vals[end_index] < 0.1:
            end_index -= 1

        num_rows = end_index - start_index + 1
        average_power = sum(power_vals[start_index : end_index + 1])/num_rows
        dynamic_power[module] = average_power
    return dynamic_power, num_rows * 100


def get_average_power_ranged(workload, start_time, end_time):
    dynamic_power = {}
    modules = ["gemmini", "spad", "acc", "mesh"]
    for module in modules:
        df = pd.read_csv(f"{data_dir}/joules_output/{workload}/{module}_power_plot.csv")
        sim_times = df['sim_time'].tolist()
        power_vals = df['power'].tolist()

        # Dynamic Power Computation
        start_index = sim_times.index(start_time)
        end_index = sim_times.index(end_time)
        num_rows = end_index - start_index + 1
        dynamic_power[module] = sum(power_vals[start_index : end_index + 1])/num_rows
    return dynamic_power, num_rows * 100


def get_average_power_full(workload):
    dynamic_power = {}
    modules = ["gemmini", "spad", "acc", "mesh"]
    for module in modules:
        df = pd.read_csv(f"{data_dir}/joules_output/{workload}/{module}_power_plot.csv")
        sim_times = df['sim_time'].tolist()
        power_vals = df['power'].tolist()

        # Dynamic Power Computation
        start_index = 2
        end_index = len(sim_times) - 1
        num_rows = end_index - start_index + 1
        dynamic_power[module] = sum(power_vals[start_index : end_index + 1])/num_rows
    return dynamic_power, num_rows * 100

def get_average_power_joules(workload):    
    plot_df = pd.read_csv(f"{data_dir}/joules_output/{workload}/gemmini_power_plot.csv")
    sim_times = plot_df['sim_time'].tolist()
    num_rows = len(sim_times) - 2

    dynamic_power = {}
    power_df = create_joules_df(workload)
    base_module_path = "/ChipTop/system/tile_prci_domain/tile_reset_domain_tile"
    submodule_breakdown = {
        "gemmini": [f"{base_module_path}/gemmini"],
        "spad": [f"{base_module_path}/gemmini/spad/spad_mems_{i}" for i in range(4)],
        "acc": [f"{base_module_path}/gemmini/spad/acc_mems_{i}" for i in range(2)],
        "mesh": [f"{base_module_path}/gemmini/ex_controller/mesh/mesh"]
    }

    for module in submodule_breakdown.keys():
        dynamic_power[module] = 0
        for submodule in submodule_breakdown[module]:
            row = power_df[power_df['Instance'] == submodule]
            dynamic_power[module] += float(row['Switching'].values[0]) + float(row['Internal'].values[0])
    return dynamic_power, num_rows * 100

def create_joules_df(workload):
    joules_report_path = f"{data_dir}/joules_output/{workload}/{workload}-baremetal-gemmini.hier.power.rpt"
    try:
        with open(joules_report_path, 'r') as file:
            data = file.read()
    except FileNotFoundError:
        print(f"Error: File not found at {joules_report_path}")
    except Exception as e:
        print(f"An error occurred: {e}")
    
    # Split the data into lines and filter out unnecessary lines
    lines = data.split("\n")
    filtered_lines = [line.strip() for line in lines if line.strip() and not line.startswith("-") and not line.startswith("Power Unit")]

    # Process data rows
    rows = []
    for line in filtered_lines[1:]:  # Skip the first row (original table headers)
        parts = line.split()
        if len(parts) < 8:
            continue  # Skip malformed lines

        cells = parts[0]
        pct_cells = parts[1]
        leakage = parts[2]
        internal = parts[3]
        switching = parts[4]
        total = parts[5]
        lvl = parts[6]
        instance = " ".join(parts[7:])  # The rest is the instance path

        rows.append([cells, pct_cells, leakage, internal, switching, total, lvl, instance])

    # Save DataFrame
    df = pd.DataFrame(rows, columns=["Cells", "Pct_cells", "Leakage", "Internal", "Switching", "Total", "Lvl", "Instance"])
    return df

# workloads = {
#     "matmul_1": {"start_time": 47505.0, "end_time": 68505.0},
#     "matmul_2": {"start_time": 94505.0, "end_time": 157505.0},
#     "matmul_3": {"start_time": 449505.0, "end_time": 663505.0},
#     "matmul_4": {"start_time": 517505.0, "end_time": 954505.0},
#     "matmul_5": {"start_time": 423505.0, "end_time": 640505.0},
#     "matmul_6": {"start_time": 94505.0, "end_time": 180505.0},
#     "matmul_7": {"start_time": 444505.0, "end_time": 620505.0},
#     "matmul_8": {"start_time": 868505.0, "end_time": 1394505.0},
#     "matmul_9": {"start_time": 217505.0, "end_time": 349505.0},
#     "matmul_10": {"start_time": 1029505.0, "end_time": 1548505.0},
#     "mlp_1": {"start_time": 1087505.0, "end_time": 1653505.0},
#     "mlp_2": {"start_time": 756505.0, "end_time": 1372505.0},
#     "mlp_3": {"start_time": 130505.0, "end_time": 214505.0},
#     "mlp_4": {"start_time": 819505.0, "end_time": 1697505.0},
#     "mlp_5": {"start_time": 1087505.0, "end_time": 1708505.0},
#     "mlp_6": {"start_time": 1307505.0, "end_time": 2136505.0},
#     "conv_1": {"start_time": 126505.0, "end_time": 513505.0},
#     "conv_2": {"start_time": 37505.0, "end_time": 98505.0},
#     "conv_3": {"start_time": 39505.0, "end_time": 455505.0},
#     "conv_4": {"start_time": 41505.0, "end_time": 99505.0},
#     "conv_5": {"start_time": 273505.0, "end_time": 975505.0},
#     "conv_6": {"start_time": 224505.0, "end_time": 633505.0},
    # "mvin_microbenchmark": {"start_time": 35505.0, "end_time": 4811505.0},
    # "mvout_microbenchmark": {"start_time": 46505.0, "end_time": 3328505.0},
    # "preload_and_compute_microbenchmark": {"start_time": 44505.0, "end_time": 1324505.0},

    # "mvin_microbenchmark_basic": {"start_time": 17505.0, "end_time": 1697505.0},
    # "mvin_microbenchmark_basic_1_col": {"start_time": 14505.0, "end_time": 2446505.0},
    # "mvin_microbenchmark_basic_2_col": {"start_time": 13505.0, "end_time": 2445505.0},
    # "mvin_microbenchmark_basic_4_col": {"start_time": 14505.0, "end_time": 2446505.0},
    # "mvin_microbenchmark_basic_8_col": {"start_time": 15505.0, "end_time": 2054505.0},
    # "mvin_microbenchmark_basic_16_col": {"start_time": 17505.0, "end_time": 1696505.0},

    # "mvout_microbenchmark_basic": {"start_time": 27505.0, "end_time": 1921505.0},
    # "mvout_microbenchmark_basic_1_col": {"start_time": 22505.0, "end_time": 2418505.0},
    # "mvout_microbenchmark_basic_2_col": {"start_time": 24505.0, "end_time": 2466505.0},
    # "mvout_microbenchmark_basic_4_col": {"start_time": 25505.0, "end_time": 2491505.0},
    # "mvout_microbenchmark_basic_8_col": {"start_time": 25505.0, "end_time": 2188505.0},
    # "mvout_microbenchmark_basic_16_col": {"start_time": 28505.0, "end_time": 1927505.0},

    # "preload_and_compute_microbenchmark_basic": {"start_time": 25505.0, "end_time": 343505.0},

    # # DIM x DIM by DIM x X
    # "preload_and_compute_microbenchmark_basic_B_1_col": {"start_time": 23505.0, "end_time": 340505.0},
    # "preload_and_compute_microbenchmark_basic_B_2_col": {"start_time": 22505.0, "end_time": 340505.0},
    # "preload_and_compute_microbenchmark_basic_B_4_col": {"start_time": 23505.0, "end_time": 340505.0},
    # "preload_and_compute_microbenchmark_basic_B_8_col": {"start_time": 23505.0, "end_time": 341505.0},
    # "preload_and_compute_microbenchmark_basic_B_16_col": {"start_time": 25505.0, "end_time": 342505.0},

    # # X x DIM by DIM x DIM,
    # "preload_and_compute_microbenchmark_basic_A_1_row": {"start_time": 20505.0, "end_time": 338505.0},
    # "preload_and_compute_microbenchmark_basic_A_2_row": {"start_time": 20505.0, "end_time": 338505.0},
    # "preload_and_compute_microbenchmark_basic_A_4_row": {"start_time": 21505.0, "end_time": 339505.0},
    # "preload_and_compute_microbenchmark_basic_A_8_row": {"start_time": 22505.0, "end_time": 340505.0},
    # "preload_and_compute_microbenchmark_basic_A_16_row": {"start_time": 25505.0, "end_time": 342505.0},

    # # X x DIM by DIM x X
    # "preload_and_compute_microbenchmark_basic_1_outer": {"start_time": 17505.0, "end_time": 334505.0},
    # "preload_and_compute_microbenchmark_basic_1_2_outer": {"start_time": 18505.0, "end_time": 335505.0},
    # "preload_and_compute_microbenchmark_basic_1_4_outer": {"start_time": 19505.0, "end_time": 336505.0},
    # "preload_and_compute_microbenchmark_basic_1_8_outer": {"start_time": 20505.0, "end_time": 337505.0},

    # "preload_and_compute_microbenchmark_basic_2_1_outer": {"start_time": 18505.0, "end_time": 335505.0},
    # "preload_and_compute_microbenchmark_basic_2_outer": {"start_time": 18505.0, "end_time": 335505.0},
    # "preload_and_compute_microbenchmark_basic_2_4_outer": {"start_time": 19505.0, "end_time": 336505.0},
    # "preload_and_compute_microbenchmark_basic_2_8_outer": {"start_time": 19505.0, "end_time": 337505.0},

    # "preload_and_compute_microbenchmark_basic_4_1_outer": {"start_time": 19505.0, "end_time": 337505.0},
    # "preload_and_compute_microbenchmark_basic_4_2_outer": {"start_time": 19505.0, "end_time": 336505.0},
    # "preload_and_compute_microbenchmark_basic_4_outer": {"start_time": 20505.0, "end_time": 337505.0},
    # "preload_and_compute_microbenchmark_basic_4_8_outer": {"start_time": 20505.0, "end_time": 337505.0},

    # "preload_and_compute_microbenchmark_basic_8_1_outer": {"start_time": 20505.0, "end_time": 338505.0},
    # "preload_and_compute_microbenchmark_basic_8_2_outer": {"start_time": 20505.0, "end_time": 337505.0},
    # "preload_and_compute_microbenchmark_basic_8_4_outer": {"start_time": 21505.0, "end_time": 339505.0},
    # "preload_and_compute_microbenchmark_basic_8_outer": {"start_time": 21505.0, "end_time": 339505.0},

    # "preload_and_compute_microbenchmark_basic_16_outer": {"start_time": 25505.0, "end_time": 342505.0},

    # # DIM x X by X x DIM
    # "preload_and_compute_microbenchmark_basic_1_inner": {"start_time": 16505.0, "end_time": 334505.0},
    # "preload_and_compute_microbenchmark_basic_2_inner": {"start_time": 18505.0, "end_time": 335505.0},
    # "preload_and_compute_microbenchmark_basic_4_inner": {"start_time": 19505.0, "end_time": 337505.0},
    # "preload_and_compute_microbenchmark_basic_8_inner": {"start_time": 21505.0, "end_time": 338505.0},
    # "preload_and_compute_microbenchmark_basic_16_inner": {"start_time": 25505.0, "end_time": 342505.0},

    # "compute_accumulated_microbenchmark_basic": {"start_time": 25505.0, "end_time": 183505.0},
# }