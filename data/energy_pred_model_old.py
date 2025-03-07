import os
import re
import numpy as np
import pandas as pd
from sklearn import linear_model
import matplotlib.pyplot as plt
import scipy

clk_period = 10
frequency = 100 * (10 ** 6)

workloads = {
    "matmul_1": {"start_time": 47505.0, "end_time": 68505.0},
    "matmul_2": {"start_time": 94505.0, "end_time": 157505.0},
    "matmul_3": {"start_time": 449505.0, "end_time": 663505.0},
    "matmul_4": {"start_time": 517505.0, "end_time": 954505.0},
    "matmul_5": {"start_time": 423505.0, "end_time": 640505.0},
    "matmul_6": {"start_time": 94505.0, "end_time": 180505.0},
    "matmul_7": {"start_time": 444505.0, "end_time": 620505.0},
    "matmul_8": {"start_time": 868505.0, "end_time": 1394505.0},
    "matmul_9": {"start_time": 217505.0, "end_time": 349505.0},
    "matmul_10": {"start_time": 1029505.0, "end_time": 1548505.0},
    "mlp_1": {"start_time": 1087505.0, "end_time": 1653505.0},
    "mlp_2": {"start_time": 756505.0, "end_time": 1372505.0},
    "mlp_3": {"start_time": 130505.0, "end_time": 214505.0},
    "mlp_4": {"start_time": 819505.0, "end_time": 1697505.0},
    "mlp_5": {"start_time": 1087505.0, "end_time": 1708505.0},
    "mlp_6": {"start_time": 1307505.0, "end_time": 2136505.0},
    "conv_1": {"start_time": 126505.0, "end_time": 513505.0},
    "conv_2": {"start_time": 37505.0, "end_time": 98505.0},
    "conv_3": {"start_time": 39505.0, "end_time": 455505.0},
    "conv_4": {"start_time": 41505.0, "end_time": 99505.0},
    "conv_5": {"start_time": 273505.0, "end_time": 975505.0},
    "conv_6": {"start_time": 224505.0, "end_time": 633505.0},
    "mvin_microbenchmark": {"start_time": 35505.0, "end_time": 4811505.0},
    "mvout_microbenchmark": {"start_time": 46505.0, "end_time": 3328505.0},
    "preload_and_compute_microbenchmark": {"start_time": 44505.0, "end_time": 1324505.0},

    "mvin_microbenchmark_basic": {"start_time": 17505.0, "end_time": 1697505.0},
    "mvin_microbenchmark_basic_1_col": {"start_time": 14505.0, "end_time": 2446505.0},
    "mvin_microbenchmark_basic_2_col": {"start_time": 13505.0, "end_time": 2445505.0},
    "mvin_microbenchmark_basic_4_col": {"start_time": 14505.0, "end_time": 2446505.0},
    "mvin_microbenchmark_basic_8_col": {"start_time": 15505.0, "end_time": 2054505.0},
    "mvin_microbenchmark_basic_16_col": {"start_time": 17505.0, "end_time": 1696505.0},

    "mvout_microbenchmark_basic": {"start_time": 27505.0, "end_time": 1921505.0},
    "mvout_microbenchmark_basic_1_col": {"start_time": 22505.0, "end_time": 2418505.0},
    "mvout_microbenchmark_basic_2_col": {"start_time": 24505.0, "end_time": 2466505.0},
    "mvout_microbenchmark_basic_4_col": {"start_time": 25505.0, "end_time": 2491505.0},
    "mvout_microbenchmark_basic_8_col": {"start_time": 25505.0, "end_time": 2188505.0},
    "mvout_microbenchmark_basic_16_col": {"start_time": 28505.0, "end_time": 1927505.0},

    "preload_and_compute_microbenchmark_basic": {"start_time": 25505.0, "end_time": 343505.0},

    # DIM x DIM by DIM x X
    "preload_and_compute_microbenchmark_basic_B_1_col": {"start_time": 23505.0, "end_time": 340505.0},
    "preload_and_compute_microbenchmark_basic_B_2_col": {"start_time": 22505.0, "end_time": 340505.0},
    "preload_and_compute_microbenchmark_basic_B_4_col": {"start_time": 23505.0, "end_time": 340505.0},
    "preload_and_compute_microbenchmark_basic_B_8_col": {"start_time": 23505.0, "end_time": 341505.0},
    "preload_and_compute_microbenchmark_basic_B_16_col": {"start_time": 25505.0, "end_time": 342505.0},

    # X x DIM by DIM x DIM,
    "preload_and_compute_microbenchmark_basic_A_1_row": {"start_time": 20505.0, "end_time": 338505.0},
    "preload_and_compute_microbenchmark_basic_A_2_row": {"start_time": 20505.0, "end_time": 338505.0},
    "preload_and_compute_microbenchmark_basic_A_4_row": {"start_time": 21505.0, "end_time": 339505.0},
    "preload_and_compute_microbenchmark_basic_A_8_row": {"start_time": 22505.0, "end_time": 340505.0},
    "preload_and_compute_microbenchmark_basic_A_16_row": {"start_time": 25505.0, "end_time": 342505.0},

    # X x DIM by DIM x X
    "preload_and_compute_microbenchmark_basic_1_outer": {"start_time": 17505.0, "end_time": 334505.0},
    "preload_and_compute_microbenchmark_basic_1_2_outer": {"start_time": 18505.0, "end_time": 335505.0},
    "preload_and_compute_microbenchmark_basic_1_4_outer": {"start_time": 19505.0, "end_time": 336505.0},
    "preload_and_compute_microbenchmark_basic_1_8_outer": {"start_time": 20505.0, "end_time": 337505.0},

    "preload_and_compute_microbenchmark_basic_2_1_outer": {"start_time": 18505.0, "end_time": 335505.0},
    "preload_and_compute_microbenchmark_basic_2_outer": {"start_time": 18505.0, "end_time": 335505.0},
    "preload_and_compute_microbenchmark_basic_2_4_outer": {"start_time": 19505.0, "end_time": 336505.0},
    "preload_and_compute_microbenchmark_basic_2_8_outer": {"start_time": 19505.0, "end_time": 337505.0},

    "preload_and_compute_microbenchmark_basic_4_1_outer": {"start_time": 19505.0, "end_time": 337505.0},
    "preload_and_compute_microbenchmark_basic_4_2_outer": {"start_time": 19505.0, "end_time": 336505.0},
    "preload_and_compute_microbenchmark_basic_4_outer": {"start_time": 20505.0, "end_time": 337505.0},
    "preload_and_compute_microbenchmark_basic_4_8_outer": {"start_time": 20505.0, "end_time": 337505.0},

    "preload_and_compute_microbenchmark_basic_8_1_outer": {"start_time": 20505.0, "end_time": 338505.0},
    "preload_and_compute_microbenchmark_basic_8_2_outer": {"start_time": 20505.0, "end_time": 337505.0},
    "preload_and_compute_microbenchmark_basic_8_4_outer": {"start_time": 21505.0, "end_time": 339505.0},
    "preload_and_compute_microbenchmark_basic_8_outer": {"start_time": 21505.0, "end_time": 339505.0},

    "preload_and_compute_microbenchmark_basic_16_outer": {"start_time": 25505.0, "end_time": 342505.0},

    # DIM x X by X x DIM
    "preload_and_compute_microbenchmark_basic_1_inner": {"start_time": 16505.0, "end_time": 334505.0},
    "preload_and_compute_microbenchmark_basic_2_inner": {"start_time": 18505.0, "end_time": 335505.0},
    "preload_and_compute_microbenchmark_basic_4_inner": {"start_time": 19505.0, "end_time": 337505.0},
    "preload_and_compute_microbenchmark_basic_8_inner": {"start_time": 21505.0, "end_time": 338505.0},
    "preload_and_compute_microbenchmark_basic_16_inner": {"start_time": 25505.0, "end_time": 342505.0},

    "compute_accumulated_microbenchmark_basic": {"start_time": 25505.0, "end_time": 183505.0},

    # "mvout_microbenchmark_basic": {"start_time": 27505.0, "end_time": 1921505.0},
    # "preload_and_compute_microbenchmark_basic": {"start_time": 25505.0, "end_time": 343505.0},
    # "compute_accumulated_microbenchmark_basic": {"start_time": 25505.0, "end_time": 183505.0},

    # "mvin_microbenchmark_basic_100": {"start_time": 15505.0, "end_time": 185505.0},
    # "mvout_microbenchmark_basic_100": {"start_time": 26505.0, "end_time": 214505.0},
    # "preload_and_compute_microbenchmark_basic_100": {"start_time": 24505.0, "end_time": 56505.0}
}

workload_data_table = {}

def get_inst_breakdown(workload):
    inst_count = {
        'mvin': 0,
        'mvout': 0,
        'compute_preloaded': 0,
        'compute_accumulated': 0,
    }

    spike_output = f"spike_output/{workload}_spike.log"
    with open(spike_output, 'r') as file:
        for line in file:
            if "mvin" in line:
                inst_count["mvin"] += 1
            elif "mvout" in line:
                inst_count["mvout"] += 1
            elif "compute_preloaded" in line:
                inst_count["compute_preloaded"] += 1
            elif "compute_accumulated" in line:
                inst_count["compute_accumulated"] += 1

    return inst_count


def get_matrices_dims(workload):
    microbenchmark_dir = os.path.dirname(__file__) + "/../scripts/templates/bareMetalC"
    define_pattern = re.compile(r'#define\s+(\w+)\s+(\d+)')
    sizes = {}
    matrices = []

    with open(f"{microbenchmark_dir}/{workload}.c", "r") as file:
        lines = file.readlines()

    for line in lines:
        match_message = define_pattern.match(line)
        matrix_match = re.match(r"// Matrix (\w+), ([^}]+) Setup", line.strip())
    
        if match_message:
            define_name = match_message.group(1)
            define_value = int(match_message.group(2))
            sizes[define_name] = define_value
        
        elif matrix_match:
            matrix_name = matrix_match.group(1)
            matrix_dims = matrix_match.group(2).split(', ')
            matrix_dims_values = [sizes[matrix_dim] for matrix_dim in matrix_dims]

            if len(matrix_dims) == 2:
                matrix_rows, matrix_columns = matrix_dims[0], matrix_dims[1]
                matrix_rows_nums, matrix_columns_nums = matrix_dims_values[0], matrix_dims_values[1]
                matrices.append(f"{matrix_rows_nums}x{matrix_columns_nums}")
            
            elif len(matrix_dims) == 4:
                matrix_dim_1, matrix_dim_2, matrix_dim_3, matrix_dim_4 = matrix_dims[0], matrix_dims[1], matrix_dims[2], matrix_dims[3]
                matrix_dim_1_nums, matrix_dim_2_nums, matrix_dim_3_nums, matrix_dim_4_nums = matrix_dims_values[0], matrix_dims_values[1], matrix_dims_values[2], matrix_dims_values[3]
                matrices.append(f"{matrix_dim_1_nums}x{matrix_dim_2_nums}x{matrix_dim_3_nums}x{matrix_dim_4_nums}")
    
    return matrices


def extract_data(table, workload):
    report_name = f"{workload}-baremetal"

    # Cycle Count Extraction
    sim_report_path = f"vcs_output/{report_name}.log"
    phrase_pattern_1 = r"Setup cycles taken: (\d+)"
    phrase_pattern_2 = r"Cycles taken: (\d+)"
    with open(sim_report_path, 'r') as file:
        file_content = file.read()
        match_1 = re.search(phrase_pattern_1, file_content)
        match_2 = re.search(phrase_pattern_2, file_content)
        setup_cycle_count = int(match_1.group(1))
        cycle_count = int(match_2.group(1))

    inst_count = get_inst_breakdown(workload)
    matrices_dims = get_matrices_dims(workload)
    
    # Dynamic Power/Energy Extraction
    dynamic_power = {}
    riemann_dynamic_energy = {}
    modules = ["gemmini", "spad", "acc", "mesh"]
    for module in modules:
        df = pd.read_csv(f"joules_output/{workload}/{module}_power_plot.csv")
        sim_times = df['sim_time'].tolist()
        power = df['power'].tolist()

        # Dynamic Power Setup 
        start_index = sim_times.index(table[workload]["start_time"])
        end_index = sim_times.index(table[workload]["end_time"])
        num_rows = end_index - start_index + 1

        # Dynamic Power Computation
        dynamic_power[module] = sum(power[start_index : end_index + 1])/num_rows

        # Riemann Dynamic Energy Computation
        rectangle_width = (sim_times[2] - sim_times[1])/clk_period
        riemann_dynamic_energy[module] = sum(power[i] * (10 ** 3) * (rectangle_width/frequency) for i in range(1, len(sim_times) - 1))

    # Average Dynamic Energy Computation
    avg_dynamic_energy = {module : dynamic_power[module] * (10 ** 3) * (cycle_count/frequency) for module in modules}

    extracted_data = {
        "setup_cycle_count": setup_cycle_count,
        "cycle_count": cycle_count,
        "inst_count": inst_count,
        "matrices_dims": matrices_dims,
        "avg_dynamic_power": dynamic_power,
        "avg_dynamic_energy": avg_dynamic_energy,
        "riemann_dynamic_energy": riemann_dynamic_energy
    }
    return extracted_data


def create_workload_data_table():
    for workload in workloads.keys():
        workload_data_table[workload] = extract_data(workloads, workload)


def epi_regression_model(energy_computation_type):
    models = {}
    for module in ["spad", "acc", "mesh"]:
        X = []
        y = []
        for workload in workload_data_table.keys():
            X.append(list(workload_data_table[workload]["inst_count"].values()))
            y.append(workload_data_table[workload][energy_computation_type][module])
        lin_reg = linear_model.LinearRegression()
        models[module] = lin_reg.fit(X, y)
    return models


def test_energy(test_program):
    test_program_data = workload_data_table[test_program]
    del workload_data_table[test_program]
    return test_program_data


def predict_energy(models, inst_counts):
    total_energy = 0
    dynamic_energy = {}
    for module in models.keys():
        module_dynamic_energy = models[module].predict([inst_counts])
        dynamic_energy[module] = module_dynamic_energy
        total_energy += module_dynamic_energy
    return dynamic_energy, total_energy


def predict_energy_micro(inst_counts, microbenchmark_type, energy_computation_type):
    mvin_count, mvout_count, compute_count = inst_counts[0], inst_counts[1], inst_counts[2] + inst_counts[3]

    mvin_energy = workload_data_table[f"mvin_microbenchmark{microbenchmark_type}"][energy_computation_type]
    mvout_energy = workload_data_table[f"mvout_microbenchmark{microbenchmark_type}"][energy_computation_type]
    compute_energy = workload_data_table[f"preload_and_compute_microbenchmark{microbenchmark_type}"][energy_computation_type]

    microbenchmark_mvin_count = workload_data_table[f"mvin_microbenchmark{microbenchmark_type}"]["inst_count"]["mvin"]
    microbenchmark_mvout_count = workload_data_table[f"mvout_microbenchmark{microbenchmark_type}"]["inst_count"]["mvout"]
    microbenchmark_compute_preloaded_count = workload_data_table[f"preload_and_compute_microbenchmark{microbenchmark_type}"]["inst_count"]["compute_preloaded"]
    microbenchmark_compute_accumulated_count = workload_data_table[f"preload_and_compute_microbenchmark{microbenchmark_type}"]["inst_count"]["compute_accumulated"]
    microbenchmark_compute_count = microbenchmark_compute_preloaded_count + microbenchmark_compute_accumulated_count

    mvin_epi = {module : mvin_energy[module]/microbenchmark_mvin_count for module in mvin_energy}
    mvout_epi = {module : mvout_energy[module]/microbenchmark_mvout_count for module in mvout_energy}
    compute_epi = {module : compute_energy[module]/microbenchmark_compute_count for module in compute_energy}

    pred_spad_energy = mvin_count * mvin_epi["spad"] + mvout_count * mvout_epi["spad"] + compute_count * compute_epi["spad"]
    pred_acc_energy = mvin_count * mvin_epi["acc"] + mvout_count * mvout_epi["acc"] + compute_count * compute_epi["acc"]
    pred_mesh_energy = mvin_count * mvin_epi["mesh"] + mvout_count * mvout_epi["mesh"] + compute_count * compute_epi["mesh"]
    pred_total_energy = mvin_count * mvin_epi["gemmini"] + mvout_count * mvout_epi["gemmini"] + compute_count * compute_epi["gemmini"]

    return pred_spad_energy, pred_acc_energy, pred_mesh_energy, pred_total_energy


def predict_energy_micro_basic(inst_counts, energy_computation_type):
    mvin_count, mvout_count, compute_preloaded_count, compute_accumulated_count = inst_counts[0], inst_counts[1], inst_counts[2], inst_counts[3]

    mvin_energy = workload_data_table[f"mvin_microbenchmark_basic"][energy_computation_type]
    mvout_energy = workload_data_table[f"mvout_microbenchmark_basic"][energy_computation_type]
    compute_preloaded_energy = workload_data_table[f"preload_and_compute_microbenchmark_basic"][energy_computation_type]
    compute_accumulated_energy = workload_data_table[f"compute_accumulated_microbenchmark_basic"][energy_computation_type]

    microbenchmark_mvin_count = workload_data_table[f"mvin_microbenchmark_basic"]["inst_count"]["mvin"]
    microbenchmark_mvout_count = workload_data_table[f"mvout_microbenchmark_basic"]["inst_count"]["mvout"]
    microbenchmark_compute_preloaded_count = workload_data_table[f"preload_and_compute_microbenchmark_basic"]["inst_count"]["compute_preloaded"]
    microbenchmark_compute_accumulated_count = workload_data_table[f"compute_accumulated_microbenchmark_basic"]["inst_count"]["compute_accumulated"]

    mvin_epi = {module : mvin_energy[module]/microbenchmark_mvin_count for module in mvin_energy}
    mvout_epi = {module : mvout_energy[module]/microbenchmark_mvout_count for module in mvout_energy}
    compute_preloaded_epi = {module : compute_preloaded_energy[module]/microbenchmark_compute_preloaded_count for module in compute_preloaded_energy}
    compute_accumulated_epi = {module : compute_accumulated_energy[module]/microbenchmark_compute_accumulated_count for module in compute_accumulated_energy}

    pred_spad_energy = mvin_count * mvin_epi["spad"] + mvout_count * mvout_epi["spad"] + compute_preloaded_count * compute_preloaded_epi["spad"] + compute_accumulated_count * compute_accumulated_epi["spad"]
    pred_acc_energy = mvin_count * mvin_epi["acc"] + mvout_count * mvout_epi["acc"] + compute_preloaded_count * compute_preloaded_epi["acc"] + compute_accumulated_count * compute_accumulated_epi["acc"]
    pred_mesh_energy = mvin_count * mvin_epi["mesh"] + mvout_count * mvout_epi["mesh"] + compute_preloaded_count * compute_preloaded_epi["mesh"] + compute_accumulated_count * compute_accumulated_epi["mesh"]
    pred_total_energy = mvin_count * mvin_epi["gemmini"] + mvout_count * mvout_epi["gemmini"] + compute_preloaded_count * compute_preloaded_epi["gemmini"] + compute_accumulated_count * compute_accumulated_epi["gemmini"]

    return pred_spad_energy, pred_acc_energy, pred_mesh_energy, pred_total_energy


error_func = lambda pred, actual : actual/pred

energy_computation_type = "avg_dynamic_energy"

create_workload_data_table()
# for workload in workload_data_table.keys():
#     inst_counts = workload_data_table[workload]["inst_count"]
#     dynamic_energy = workload_data_table[workload][energy_computation_type]
#     print(f"{workload}: {inst_counts}, {dynamic_energy}")

df_columns = [
    'workload',
    'mvin_count',
    'mvout_count',
    'preload_compute_count',
    'accumulated_compute_count', 
    'compute_count',
    'uses_accumulated_computes',
    'spad_energy',
    'acc_energy',
    'mesh_energy',
    'gemmini_energy',
    'pred_spad_energy',
    'pred_acc_energy',
    'pred_mesh_energy',
    'pred_gemmini_energy',
    'spad_error',
    'acc_error',
    'mesh_error',
    'gemmini_error'
]
df = pd.DataFrame(columns=df_columns)

microbenchmark_df_columns = [
    'microbenchmark',
    'cycle_count',
    'spad_power',
    'acc_power',
    'mesh_power',
    'gemmini_power',
    'spad_energy',
    'acc_energy',
    'mesh_energy',
    'gemmini_energy'
]
microbenchmark_df = pd.DataFrame(columns=microbenchmark_df_columns)

for workload in workload_data_table.keys():
    if "mvin" not in workload and "mvout" not in workload and "preload_and_compute" not in workload and "compute_accumulated" not in workload:
        inst_counts = list(workload_data_table[workload]["inst_count"].values())
        input_matrix_dims = workload_data_table[workload]["matrices_dims"][0]
        pred_spad_energy, pred_acc_energy, pred_mesh_energy, pred_gemmini_energy = predict_energy_micro(inst_counts, "_basic", energy_computation_type)
        spad_energy, acc_energy, mesh_energy, gemmini_energy = workload_data_table[workload][energy_computation_type]["spad"], workload_data_table[workload][energy_computation_type]["acc"], workload_data_table[workload][energy_computation_type]["mesh"], workload_data_table[workload][energy_computation_type]["gemmini"]

        spad_error = round(error_func(pred_spad_energy, spad_energy), 2)
        acc_error = round(error_func(pred_acc_energy, acc_energy), 2)
        mesh_error = round(error_func(pred_mesh_energy, mesh_energy), 2)
        gemmini_error = round(error_func(pred_gemmini_energy, gemmini_energy), 2)

        new_row = pd.DataFrame([{
            'workload': workload,
            'input_matrix_dims': input_matrix_dims,
            'mvin_count': inst_counts[0],
            'mvout_count': inst_counts[1],
            'preload_compute_count': inst_counts[2],
            'accumulated_compute_count': inst_counts[3],
            'compute_count': inst_counts[2] + inst_counts[3],
            'uses_accumulated_computes': inst_counts[3] != 0,
            'spad_energy': spad_energy,
            'acc_energy': acc_energy,
            'mesh_energy': mesh_energy,
            'gemmini_energy': gemmini_energy,
            'pred_spad_energy': pred_spad_energy,
            'pred_acc_energy': pred_acc_energy,
            'pred_mesh_energy': pred_mesh_energy,
            'pred_gemmini_energy': pred_gemmini_energy,
            'spad_error': spad_error,
            'acc_error': acc_error,
            'mesh_error': mesh_error,
            'gemmini_error': gemmini_error
        }])
        df = pd.concat([df, new_row], ignore_index=True)

        # print(f"**{workload}**")
        # print(f"inst_counts: {workload_data_table[workload]["inst_count"]}")
        # print(f"matrices_dims: {workload_data_table[workload]["matrices_dims"]}")
        # # print(f"cycles: {workload_data_table[workload]["cycle_count"]}")
        # # print(f"spad_power:\t  {workload_data_table[workload]["avg_dynamic_power"]["spad"]},\tacc_power:\t {workload_data_table[workload]["avg_dynamic_power"]["acc"]},\tmesh_power:\t  {workload_data_table[workload]["avg_dynamic_power"]["mesh"]},\ttotal_power:\t   {workload_data_table[workload]["avg_dynamic_power"]["gemmini"]}")
        # print(f"spad_energy:\t  {spad_energy},\tacc_energy:\t {acc_energy},\tmesh_energy:\t  {mesh_energy},\tgemmini_energy:\t   {gemmini_energy}")
        # print(f"pred_spad_energy: {pred_spad_energy},\tpred_acc_energy: {pred_acc_energy},\tpred_mesh_energy: {pred_mesh_energy},\tpred_gemmini_energy: {pred_gemmini_energy}")
        # print(f"spad error: {spad_error}%,\t\t\tacc_error: {acc_error}%,\t\t\tmesh_error: {mesh_error}%,\t\t\tgemmini_error: {gemmini_error}%\n")
    else:
        inst_counts = list(workload_data_table[workload]["inst_count"].values())
        spad_power, acc_power, mesh_power, gemmini_power = workload_data_table[workload]["avg_dynamic_power"]["spad"], workload_data_table[workload]["avg_dynamic_power"]["acc"], workload_data_table[workload]["avg_dynamic_power"]["mesh"], workload_data_table[workload]["avg_dynamic_power"]["gemmini"]
        spad_energy, acc_energy, mesh_energy, gemmini_energy = workload_data_table[workload][energy_computation_type]["spad"], workload_data_table[workload][energy_computation_type]["acc"], workload_data_table[workload][energy_computation_type]["mesh"], workload_data_table[workload][energy_computation_type]["gemmini"]

        if 'preload_and_compute_microbenchmark_basic' in workload:
            new_row = pd.DataFrame([{
                'microbenchmark': workload,
                'cycle_count': workload_data_table[workload]["cycle_count"],
                'spad_power': spad_power,
                'acc_power': acc_power,
                'mesh_power': mesh_power,
                'gemmini_power': gemmini_power,
                'spad_energy': spad_energy,
                'acc_energy': acc_energy,
                'mesh_energy': mesh_energy,
                'gemmini_energy': gemmini_energy
            }])
            microbenchmark_df = pd.concat([microbenchmark_df, new_row], ignore_index=True)

        print(f"**{workload}**")
        print(f"inst_counts: {workload_data_table[workload]["inst_count"]}")
        print(f"cycles: {workload_data_table[workload]["cycle_count"]}")
        print(f"spad_power:\t  {spad_power},\tacc_power:\t {acc_power},\tmesh_power:\t  {mesh_power},\tgemmini_power:\t   {gemmini_power}")
        print(f"spad_energy:\t  {spad_energy},\tacc_energy:\t {acc_energy},\tmesh_energy:\t  {mesh_energy},\tgemmini_energy:\t   {gemmini_energy}")
        print("\n")


microbenchmark_df.to_csv('microbenchmarks.csv', index=False)
df.to_csv('workloads.csv', index=False)

modules = ['spad', 'acc', 'mesh']
for module in modules:
    # Config
    plt.figure(figsize=(30, 25))
    plt.title(f'{module} Prediction Error vs. Compute Instruction Counts')
    plt.xlabel('Compute Instructions')
    plt.ylabel('Actual/Predicted')

    # Data
    df_no_acc = df.loc[df['uses_accumulated_computes'] == False]
    df_acc = df.loc[df['uses_accumulated_computes'] == True & ~df['workload'].str.contains("conv", case=False, na=False)]
    df_conv = df.loc[df['workload'].str.contains("conv", case=False, na=False)]

    # Scatter plots
    plt.scatter(df_no_acc['compute_count'], df_no_acc[f'{module}_error'], label='No Accumulated Computes', color='r')
    plt.scatter(df_acc['compute_count'], df_acc[f'{module}_error'], label='Accumulated Computes', color='b')
    plt.scatter(df_conv['compute_count'], df_conv[f'{module}_error'], label='Convolution', color='g')

    for _, row in df.iterrows():
        x_coord = row['compute_count']
        y_coord = row[f'{module}_error']
        label = f"{row['workload']}\n{row['input_matrix_dims']}"
        
        offset = max(0.05 * abs(y_coord), 0.01)
        
        # Annotate below the point
        plt.annotate(
            label, 
            (x_coord, y_coord), 
            textcoords="offset points", 
            xytext=(0, -20),
            ha='center', 
            fontsize=8
        )

    # Lines
    x, y = np.array(df_no_acc['compute_count'], dtype=np.int64), np.array(df_no_acc[f'{module}_error'], dtype=np.float64)
    a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
    r_squared = r_value ** 2
    plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}", color='r')

    x, y = np.array(df_acc['compute_count'], dtype=np.int64), np.array(df_acc[f'{module}_error'], dtype=np.float64)
    a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
    r_squared = r_value ** 2
    plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}", color='b')

    x, y = np.array(df_conv['compute_count'], dtype=np.int64), np.array(df_conv[f'{module}_error'], dtype=np.float64)
    a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
    r_squared = r_value ** 2
    plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}", color='g')

    x, y = np.array(df['compute_count'], dtype=np.int64), np.array(df[f'{module}_error'], dtype=np.float64)
    a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
    r_squared = r_value ** 2
    plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}", linewidth=3, color='gray')

    plt.xlim(left=0)
    plt.legend()

    plt.savefig(f'_test_plots/compute_{module}_error_plot.png')
    plt.close()












df = pd.DataFrame(columns=df_columns)

for workload in workload_data_table.keys():
    if "mvin" not in workload and "mvout" not in workload and "preload_and_compute" not in workload and "compute_accumulated" not in workload:
        inst_counts = list(workload_data_table[workload]["inst_count"].values())
        input_matrix_dims = workload_data_table[workload]["matrices_dims"][0]
        pred_spad_energy, pred_acc_energy, pred_mesh_energy, pred_gemmini_energy = predict_energy_micro_basic(inst_counts, energy_computation_type)
        spad_energy, acc_energy, mesh_energy, gemmini_energy = workload_data_table[workload][energy_computation_type]["spad"], workload_data_table[workload][energy_computation_type]["acc"], workload_data_table[workload][energy_computation_type]["mesh"], workload_data_table[workload][energy_computation_type]["gemmini"]

        spad_error = round(error_func(pred_spad_energy, spad_energy), 2)
        acc_error = round(error_func(pred_acc_energy, acc_energy), 2)
        mesh_error = round(error_func(pred_mesh_energy, mesh_energy), 2)
        gemmini_error = round(error_func(pred_gemmini_energy, gemmini_energy), 2)

        new_row = pd.DataFrame([{'Column1': 'value1', 'Column2': 'value2', 'Column3': 'value3'}])


        new_row = pd.DataFrame([{
            'workload': workload,
            'input_matrix_dims': input_matrix_dims,
            'mvin_count': inst_counts[0],
            'mvout_count': inst_counts[1],
            'preload_compute_count': inst_counts[2],
            'accumulated_compute_count': inst_counts[3],
            'compute_count': inst_counts[2] + inst_counts[3],
            'uses_accumulated_computes': inst_counts[3] != 0,
            'spad_energy': spad_energy,
            'acc_energy': acc_energy,
            'mesh_energy': mesh_energy,
            'gemmini_energy': gemmini_energy,
            'pred_spad_energy': pred_spad_energy,
            'pred_acc_energy': pred_acc_energy,
            'pred_mesh_energy': pred_mesh_energy,
            'pred_gemmini_energy': pred_gemmini_energy,
            'spad_error': spad_error,
            'acc_error': acc_error,
            'mesh_error': mesh_error,
            'gemmini_error': gemmini_error
        }])
        df = pd.concat([df, new_row], ignore_index=True)

        # print(f"**{workload}**")
        # print(f"inst_counts: {workload_data_table[workload]["inst_count"]}")
        # print(f"matrices_dims: {workload_data_table[workload]["matrices_dims"]}")
        # # print(f"cycles: {workload_data_table[workload]["cycle_count"]}")
        # # print(f"spad_power:\t  {workload_data_table[workload]["avg_dynamic_power"]["spad"]},\tacc_power:\t {workload_data_table[workload]["avg_dynamic_power"]["acc"]},\tmesh_power:\t  {workload_data_table[workload]["avg_dynamic_power"]["mesh"]},\ttotal_power:\t   {workload_data_table[workload]["avg_dynamic_power"]["gemmini"]}")
        # print(f"spad_energy:\t  {spad_energy},\tacc_energy:\t {acc_energy},\tmesh_energy:\t  {mesh_energy},\tgemmini_energy:\t   {gemmini_energy}")
        # print(f"pred_spad_energy: {pred_spad_energy},\tpred_acc_energy: {pred_acc_energy},\tpred_mesh_energy: {pred_mesh_energy},\tpred_gemmini_energy: {pred_gemmini_energy}")
        # print(f"spad error: {spad_error}%,\t\t\tacc_error: {acc_error}%,\t\t\tmesh_error: {mesh_error}%,\t\t\tgemmini_error: {gemmini_error}%\n")

df.to_csv('workloads2.csv', index=False)

modules = ['spad', 'acc', 'mesh']
for module in modules:
    # Config
    plt.figure(figsize=(30, 25))
    plt.title(f'{module} Prediction Error vs. Compute Instruction Counts')
    plt.xlabel('Compute Instructions')
    plt.ylabel('Actual/Predicted')

    # Data
    df_no_acc = df.loc[df['uses_accumulated_computes'] == False]
    df_acc = df.loc[df['uses_accumulated_computes'] == True & ~df['workload'].str.contains("conv", case=False, na=False)]
    df_conv = df.loc[df['workload'].str.contains("conv", case=False, na=False)]

    # Scatter plots
    plt.scatter(df_no_acc['compute_count'], df_no_acc[f'{module}_error'], label='No Accumulated Computes', color='r')
    plt.scatter(df_acc['compute_count'], df_acc[f'{module}_error'], label='Accumulated Computes', color='b')
    plt.scatter(df_conv['compute_count'], df_conv[f'{module}_error'], label='Convolution', color='g')

    for _, row in df.iterrows():
        x_coord = row['compute_count']
        y_coord = row[f'{module}_error']
        label = f"{row['workload']}\n{row['input_matrix_dims']}"
        
        offset = max(0.05 * abs(y_coord), 0.01)
        
        # Annotate below the point
        plt.annotate(
            label, 
            (x_coord, y_coord), 
            textcoords="offset points", 
            xytext=(0, -20),
            ha='center', 
            fontsize=8
        )

    # Lines
    x, y = np.array(df_no_acc['compute_count'], dtype=np.int64), np.array(df_no_acc[f'{module}_error'], dtype=np.float64)
    a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
    r_squared = r_value ** 2
    plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}", color='r')

    x, y = np.array(df_acc['compute_count'], dtype=np.int64), np.array(df_acc[f'{module}_error'], dtype=np.float64)
    a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
    r_squared = r_value ** 2
    plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}", color='b')

    x, y = np.array(df_conv['compute_count'], dtype=np.int64), np.array(df_conv[f'{module}_error'], dtype=np.float64)
    a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
    r_squared = r_value ** 2
    plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}", color='g')

    x, y = np.array(df['compute_count'], dtype=np.int64), np.array(df[f'{module}_error'], dtype=np.float64)
    a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
    r_squared = r_value ** 2
    plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}", linewidth=3, color='gray')

    plt.xlim(left=0)
    plt.legend()

    plt.savefig(f'_test_plots2/compute_{module}_error_plot.png')
    plt.close()




# # Config
# plt.figure(figsize=(15, 10))
# plt.title("Acc Prediction Error vs. Compute Instruction Counts")
# plt.xlabel('Compute Instructions')
# plt.ylabel('Error (%)')

# # Scatter plots
# plt.scatter(compute_values, acc_error_values, label='No Accumulated Computes')
# plt.scatter(compute_values2, acc_error_values2, label='Accumulated Computes')

# # Lines
# x, y = np.array(compute_values), np.array(acc_error_values)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# x, y = np.array(compute_values2), np.array(acc_error_values2)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# x, y = np.array(compute_values + compute_values2), np.array(acc_error_values + acc_error_values2)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# plt.xlim(left=0)
# # plt.ylim(bottom=0)
# plt.legend()

# plt.savefig(f"/bwrcq/scratch/jfwang983/power-prediction/data/_test_plots/compute_acc_error_plot.png")
# plt.close()



# # Config
# plt.figure(figsize=(15, 10))
# plt.title("Mesh Prediction Error vs. Compute Instruction Counts")
# plt.xlabel('Compute Instructions')
# plt.ylabel('Error (%)')

# # Scatter plots
# plt.scatter(compute_values, mesh_error_values, label='No Accumulated Computes')
# plt.scatter(compute_values2, mesh_error_values2, label='Accumulated Computes')

# # Lines
# x, y = np.array(compute_values), np.array(mesh_error_values)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# x, y = np.array(compute_values2), np.array(mesh_error_values2)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# x, y = np.array(compute_values + compute_values2), np.array(mesh_error_values + mesh_error_values2)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# plt.xlim(left=0)
# # plt.ylim(bottom=0)
# plt.legend()

# plt.savefig(f"/bwrcq/scratch/jfwang983/power-prediction/data/_test_plots/compute_mesh_error_plot.png")
# plt.close()