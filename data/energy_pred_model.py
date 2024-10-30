import re
import numpy as np
import pandas as pd
from sklearn import linear_model
import matplotlib.pyplot as plt
import scipy

clk_period = 32
frequency = 31.25 * (10 ** 6)

workloads = {
    # "mvin_cache_hit_microbenchmark_random": {"start_time": 916816.0, "end_time": 1387216.0},
    # "mvout_microbenchmark_random": {"start_time": 916816.0, "end_time": 1387216.0},
    # "preload_and_compute_random": {"start_time": 916816.0, "end_time": 1387216.0},

    "matmul_1": {"start_time": 910416.0, "end_time": 1316816.0},
    "matmul_2": {"start_time": 337616.0, "end_time": 516816.0},
    "matmul_3": {"start_time": 1502416.0, "end_time": 2068816.0},
    "matmul_4": {"start_time": 1752016.0, "end_time": 3003216.0},
    "matmul_5": {"start_time": 1444816.0, "end_time": 2008016.0},
    "matmul_6": {"start_time": 337616.0, "end_time": 590416.0},
    "matmul_7": {"start_time": 1499216.0, "end_time": 1944016.0},
    "matmul_8": {"start_time": 2936016.0, "end_time": 4379216.0},
    "matmul_9": {"start_time": 760016.0, "end_time": 1092816.0},
    "matmul_10": {"start_time": 3435216.0, "end_time": 4897616.0},
    "mlp_1": {"start_time": 3659216.0, "end_time": 5137616.0},
    "mlp_2": {"start_time": 2529616.0, "end_time": 4241616.0},
    "mlp_3": {"start_time": 462416.0, "end_time": 712016.0},
    "mlp_4": {"start_time": 2750416.0, "end_time": 5246416.0},
    "mlp_5": {"start_time": 3691216.0, "end_time": 5483216.0},
    "mlp_6": {"start_time": 4366416.0, "end_time": 6529616.0},
    "conv_1": {"start_time": 449616.0, "end_time": 1633616.0},
    "conv_2": {"start_time": 142416.0, "end_time": 337616.0},
    "conv_3": {"start_time": 152016.0, "end_time": 1540816.0},
    "conv_4": {"start_time": 158416.0, "end_time": 337616.0},
    "conv_5": {"start_time": 939216.0, "end_time": 3112016.0},
    "conv_6": {"start_time": 779216.0, "end_time": 2046416.0},
    "mvin_microbenchmark": {"start_time": 145616.0, "end_time": 4372816.0},
    "mvout_microbenchmark": {"start_time": 152016.0, "end_time": 3569616.0},
    "preload_and_compute_microbenchmark": {"start_time": 155216.0, "end_time": 4251216.0},
    "mvin_microbenchmark_basic": {"start_time": 75216.0, "end_time": 644816.0},
    "mvout_microbenchmark_basic": {"start_time": 94416.0, "end_time": 913616.0},
    "preload_and_compute_microbenchmark_basic": {"start_time": 91216.0, "end_time": 1115216.0}

    # "test_matmul_1": {"start_time": 916816.0, "end_time": 1387216.0},
    # "test_matmul_2": {"start_time": 344016.0, "end_time": 555216.0},
    # "test_matmul_3": {"start_time": 1480016.0, "end_time": 2161616.0},
    # "test_matmul_4": {"start_time": 1780816.0, "end_time": 3428816.0},
    # "test_matmul_5": {"start_time": 1412816.0, "end_time": 2094416.0},
    # "test_matmul_6": {"start_time": 340816.0, "end_time": 644816.0},
    # "test_matmul_7": {"start_time": 1470416.0, "end_time": 2052816.0},
    # "test_matmul_8": {"start_time": 2990416.0, "end_time": 4872016.0},
    # "test_matmul_9": {"start_time": 747216.0, "end_time": 1147216.0},
    # "test_matmul_10": {"start_time": 3441616.0, "end_time": 5153616.0},
    # "test_mlp_1": {"start_time": 3659216.0, "end_time": 5137616.0}
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
    
    # Dynamic Power/Energy Extraction
    dynamic_power = riemann_dynamic_energy = {}
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
        dynamic_power[module] = 0
        for i in range(start_index, end_index + 1):
            dynamic_power[module] += power[i]
        dynamic_power[module] /= num_rows

        # Riemann Dynamic Energy Computation
        rectangle_width = (sim_times[2] - sim_times[1])/clk_period
        riemann_dynamic_energy[module] = 0
        for i in range(1, len(sim_times) - 1):
            riemann_dynamic_energy[module] += power[i] * (10 ** 3) * (rectangle_width/frequency)
    
    # Average Dynamic Energy Computation
    avg_dynamic_energy = {module : dynamic_power[module] * (10 ** 3) * (cycle_count/frequency) for module in modules}

    extracted_data = {
        "setup_cycle_count": setup_cycle_count,
        "cycle_count": cycle_count,
        "inst_count": inst_count,
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
    # mvin_count, mvout_count, compute_count = inst_counts[0], inst_counts[1], inst_counts[2] + inst_counts[3]
    # pred_spad_energy = mvin_count * 0.00436352164881 + mvout_count * 0.00012447548018 + compute_count * 0.0005624359995560516
    # pred_acc_energy = mvin_count * (8.8938355053 * (10 ** -7)) + mvout_count * 0.0101020170795 + compute_count * 0.0014094143032892677
    # pred_mesh_energy = mvin_count * 0.00000175382430932 + mvout_count * 0.0043409899905 + compute_count * 0.00556392010362
    # return pred_spad_energy, pred_acc_energy, pred_mesh_energy

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


error_func = lambda pred, actual : (actual - pred)/pred * 100

energy_computation_type = "avg_dynamic_energy"

create_workload_data_table()
# for workload in workload_data_table.keys():
#     inst_counts = workload_data_table[workload]["inst_count"]
#     dynamic_energy = workload_data_table[workload][energy_computation_type]
#     print(f"{workload}: {inst_counts}, {dynamic_energy}")

mvin_values = []
mvout_values = []
compute_values = []

mvin_values2 = []
mvout_values2 = []
compute_values2 = []

spad_values = []
pred_spad_values = []
spad_error_values = []
acc_values = []
pred_acc_values = []
acc_error_values = []
mesh_values = []
pred_mesh_values = []
mesh_error_values = []

spad_values2 = []
pred_spad_values2 = []
spad_error_values2 = []
acc_values2 = []
pred_acc_values2 = []
acc_error_values2 = []
mesh_values2 = []
pred_mesh_values2 = []
mesh_error_values2 = []

for workload in workload_data_table.keys():
    if "mvin" not in workload and "mvout" not in workload and "preload_and_compute" not in workload:
        inst_counts = list(workload_data_table[workload]["inst_count"].values())
        pred_spad_energy, pred_acc_energy, pred_mesh_energy, pred_total_energy = predict_energy_micro(inst_counts, "_basic", energy_computation_type)
        spad_energy, acc_energy, mesh_energy, total_energy = workload_data_table[workload][energy_computation_type]["spad"], workload_data_table[workload][energy_computation_type]["acc"], workload_data_table[workload][energy_computation_type]["mesh"], workload_data_table[workload][energy_computation_type]["gemmini"]

        spad_error = round(error_func(pred_spad_energy, spad_energy), 2)
        acc_error = round(error_func(pred_acc_energy, acc_energy), 2)
        mesh_error = round(error_func(pred_mesh_energy, mesh_energy), 2)
        gemmini_error = round(error_func(pred_total_energy, total_energy), 2)

        if inst_counts[3] == 0:
            mvin_values.append(inst_counts[0])
            mvout_values.append(inst_counts[1])
            compute_values.append(inst_counts[2] + inst_counts[3])

            spad_values.append(spad_energy)
            pred_spad_values.append(pred_spad_energy)
            acc_values.append(acc_energy)
            pred_acc_values.append(pred_acc_energy)
            mesh_values.append(mesh_energy)
            pred_mesh_values.append(pred_mesh_energy)

            spad_error_values.append(spad_error)
            acc_error_values.append(acc_error)
            mesh_error_values.append(mesh_error)
        else:
            mvin_values2.append(inst_counts[0])
            mvout_values2.append(inst_counts[1])
            compute_values2.append(inst_counts[2] + inst_counts[3])

            spad_values2.append(spad_energy)
            pred_spad_values2.append(pred_spad_energy)
            acc_values2.append(acc_energy)
            pred_acc_values2.append(pred_acc_energy)
            mesh_values2.append(mesh_energy)
            pred_mesh_values2.append(pred_mesh_energy)

            spad_error_values2.append(spad_error)
            acc_error_values2.append(acc_error)
            mesh_error_values2.append(mesh_error)

        print(f"**{workload}**")
        print(f"inst_counts: {workload_data_table[workload]["inst_count"]}")
        print(f"spad_energy:\t  {spad_energy},\tacc_energy:\t {acc_energy},\tmesh_energy:\t  {mesh_energy},\ttotal_energy:\t   {total_energy}")
        print(f"pred_spad_energy: {pred_spad_energy},\tpred_acc_energy: {pred_acc_energy},\tpred_mesh_energy: {pred_mesh_energy},\tpred_total_energy: {pred_total_energy}")
        print(f"spad error: {spad_error}%,\t\t\tacc_error: {acc_error}%,\t\t\tmesh_error: {mesh_error}%,\t\t\tgemmini_error: {gemmini_error}%\n")



# inst_values = {"mvin": mvin_values, "mvout": mvout_values, "compute": compute_values}
# for inst in inst_values.keys():
#     inst_value = inst_values[inst]
#     # Config
#     plt.figure(figsize=(15, 10))
#     plt.title("Energy vs. Compute Instruction Counts")
#     plt.xlabel('Compute Instructions')
#     plt.ylabel('Energy (uJ)')

#     # Linear Lines
#     energy_values = [spad_values, acc_values, mesh_values]
#     for energy_value in energy_values:
#         x, y = np.array(inst_value), np.array(energy_value)
#         a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
#         r_squared = r_value ** 2
#         plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

#     # Scatter plots
#     plt.scatter(inst_value, spad_values, label='Spad')
#     plt.scatter(inst_value, acc_values, label='Acc')
#     plt.scatter(inst_value, mesh_values, label='Mesh')

#     plt.xlim(left=0)
#     plt.ylim(bottom=0)
#     plt.legend()

#     plt.savefig(f"/bwrcq/scratch/jfwang983/power-prediction/data/_test_plots/actual_{inst}_plot.png")
#     plt.close()

#     # Config
#     plt.figure(figsize=(15, 10))
#     plt.title("Predicted Energy vs. Compute Instruction Counts")
#     plt.xlabel('Compute Instructions')
#     plt.ylabel('Predicted Energy (uJ)')

#     # Linear Lines
#     energy_values = [pred_spad_values, pred_acc_values, pred_mesh_values]
#     for energy_value in energy_values:
#         x, y = np.array(inst_value), np.array(energy_value)
#         a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
#         r_squared = r_value ** 2
#         plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

#     # Scatter plots
#     plt.scatter(inst_value, pred_spad_values, label='Spad')
#     plt.scatter(inst_value, pred_acc_values, label='Acc')
#     plt.scatter(inst_value, pred_mesh_values, label='Mesh')

#     plt.xlim(left=0)
#     plt.ylim(bottom=0)
#     plt.legend()

#     plt.savefig(f"/bwrcq/scratch/jfwang983/power-prediction/data/_test_plots/pred_{inst}_plot.png")
#     plt.close()





# Config
plt.figure(figsize=(15, 10))
plt.title("Spad Prediction Error vs. Compute Instruction Counts")
plt.xlabel('Compute Instructions')
plt.ylabel('Error (%)')

# Scatter plots
plt.scatter(compute_values, spad_error_values, label='No Accumulated Computes')
plt.scatter(compute_values2, spad_error_values2, label='Accumulated Computes')

# Lines
x, y = np.array(compute_values), np.array(spad_error_values)
a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
r_squared = r_value ** 2
plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

x, y = np.array(compute_values2), np.array(spad_error_values2)
a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
r_squared = r_value ** 2
plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

x, y = np.array(compute_values + compute_values2), np.array(spad_error_values + spad_error_values2)
a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
r_squared = r_value ** 2
plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

plt.xlim(left=0)
# plt.ylim(bottom=0)
plt.legend()

plt.savefig(f"/bwrcq/scratch/jfwang983/power-prediction/data/_test_plots/compute_spad_error_plot.png")
plt.close()



# Config
plt.figure(figsize=(15, 10))
plt.title("Acc Prediction Error vs. Compute Instruction Counts")
plt.xlabel('Compute Instructions')
plt.ylabel('Error (%)')

# Scatter plots
plt.scatter(compute_values, acc_error_values, label='No Accumulated Computes')
plt.scatter(compute_values2, acc_error_values2, label='Accumulated Computes')

# Lines
x, y = np.array(compute_values), np.array(acc_error_values)
a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
r_squared = r_value ** 2
plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

x, y = np.array(compute_values2), np.array(acc_error_values2)
a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
r_squared = r_value ** 2
plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

x, y = np.array(compute_values + compute_values2), np.array(acc_error_values + acc_error_values2)
a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
r_squared = r_value ** 2
plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

plt.xlim(left=0)
# plt.ylim(bottom=0)
plt.legend()

plt.savefig(f"/bwrcq/scratch/jfwang983/power-prediction/data/_test_plots/compute_acc_error_plot.png")
plt.close()



# Config
plt.figure(figsize=(15, 10))
plt.title("Mesh Prediction Error vs. Compute Instruction Counts")
plt.xlabel('Compute Instructions')
plt.ylabel('Error (%)')

# Scatter plots
plt.scatter(compute_values, mesh_error_values, label='No Accumulated Computes')
plt.scatter(compute_values2, mesh_error_values2, label='Accumulated Computes')

# Lines
x, y = np.array(compute_values), np.array(mesh_error_values)
a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
r_squared = r_value ** 2
plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

x, y = np.array(compute_values2), np.array(mesh_error_values2)
a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
r_squared = r_value ** 2
plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

x, y = np.array(compute_values + compute_values2), np.array(mesh_error_values + mesh_error_values2)
a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
r_squared = r_value ** 2
plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

plt.xlim(left=0)
# plt.ylim(bottom=0)
plt.legend()

plt.savefig(f"/bwrcq/scratch/jfwang983/power-prediction/data/_test_plots/compute_mesh_error_plot.png")
plt.close()






# # Config
# plt.figure(figsize=(15, 10))
# plt.title("Energy vs. Compute Instruction Counts")
# plt.xlabel('Compute Instructions')
# plt.ylabel('Energy (uJ)')

# # Scatter plots
# plt.scatter(compute_values, spad_values, label='Actual Spad')
# plt.scatter(compute_values, pred_spad_values, label='Predicted Spad')

# x, y = np.array(compute_values), np.array(spad_values)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# plt.xlim(left=0)
# plt.ylim(bottom=0)
# plt.legend()

# plt.savefig(f"/bwrcq/scratch/jfwang983/power-prediction/data/_test_plots/actual_pred_spad_plot.png")
# plt.close()


# # Config
# plt.figure(figsize=(15, 10))
# plt.title("Energy vs. Compute Instruction Counts")
# plt.xlabel('Compute Instructions')
# plt.ylabel('Energy (uJ)')

# # Scatter plots
# plt.scatter(compute_values, acc_values, label='Actual Acc')
# plt.scatter(compute_values, pred_acc_values, label='Predicted Acc')

# x, y = np.array(compute_values), np.array(acc_values)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# plt.xlim(left=0)
# plt.ylim(bottom=0)
# plt.legend()

# plt.savefig(f"/bwrcq/scratch/jfwang983/power-prediction/data/_test_plots/actual_pred_acc_plot.png")
# plt.close()


# # Config
# plt.figure(figsize=(15, 10))
# plt.title("Energy vs. Compute Instruction Counts")
# plt.xlabel('Compute Instructions')
# plt.ylabel('Energy (uJ)')

# # Scatter plots
# plt.scatter(compute_values, mesh_values, label='Actual Mesh')
# plt.scatter(compute_values, pred_mesh_values, label='Predicted Mesh')

# x, y = np.array(compute_values), np.array(mesh_values)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# plt.xlim(left=0)
# plt.ylim(bottom=0)
# plt.legend()

# plt.savefig(f"/bwrcq/scratch/jfwang983/power-prediction/data/_test_plots/actual_pred_mesh_plot.png")
# plt.close()






# test_program_data = test_energy(test_program)
# models = epi_regression_model(energy_computation_type)
# test_inst_counts = list(test_program_data["inst_count"].values())
# dynamic_energy, total_energy = predict_energy(models, test_inst_counts)
# predicted_total_energy = total_energy[0]

# actual_total_energy = test_program_data[energy_computation_type]["gemmini"]
# del test_program_data[energy_computation_type]["gemmini"]
# actual_dynamic_energy = test_program_data[energy_computation_type]

# print("\nPREDICTED:")
# predicted_spad_energy, predicted_acc_energy, predicted_mesh_energy = dynamic_energy['spad'][0], dynamic_energy['acc'][0], dynamic_energy['mesh'][0]
# print(f"{predicted_spad_energy} uJ, {predicted_acc_energy} uJ, {predicted_mesh_energy} uJ, {predicted_total_energy} uJ")
# print("\nACTUAL:")
# actual_spad_energy, actual_acc_energy, actual_mesh_energy = actual_dynamic_energy['spad'], actual_dynamic_energy['acc'], actual_dynamic_energy['mesh']
# print(f"{actual_spad_energy} uJ, {actual_acc_energy} uJ, {actual_mesh_energy} uJ, {actual_total_energy} uJ")
# print("\nERROR:")
# spad_error = error_func(predicted_spad_energy, actual_spad_energy)
# acc_error = error_func(predicted_acc_energy, actual_acc_energy)
# mesh_error = error_func(predicted_mesh_energy, actual_mesh_energy)
# total_error = error_func(predicted_total_energy, actual_total_energy)
# print(f"{spad_error}%, {acc_error}%, {mesh_error}%, {total_error}%")



# print("\n")

# print("***Validation Results***")
# for benchmark in validity_benchmark_data_table.keys():
#     predicted_energy = epi_model(validity_benchmark_data_table[benchmark]["inst_count"])
#     actual_energy_breakdown = validity_benchmark_data_table[benchmark]["dynamic_energy_breakdown"]
#     actual_energy = sum([actual_energy_breakdown[key] for key in actual_energy_breakdown.keys()])
#     percent_error = abs((actual_energy - sum(predicted_energy))/sum(predicted_energy)) * 100

#     print(benchmark)
#     print("inst counts")
#     print(validity_benchmark_data_table[benchmark]["inst_count"], "\n")
#     print("energy breakdown")
#     print(validity_benchmark_data_table[benchmark]["dynamic_energy_breakdown"], "\n")
#     print("Actual Energy:", actual_energy, "uJ")
#     print("Predicted Energy Breakdown:", predicted_energy)
#     print("Predicted Energy:", sum(predicted_energy), "uJ")
#     print(f"Percent Error: {round(percent_error, 2)}%\n\n")
