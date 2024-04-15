import os
import re
import yaml

frequency = 31.25 * (10 ** 6)

mvin_cache_hit = 0
mvin_cache_miss = 0
mvout= 0
compute = 0

def get_per_instruction_energy():
    global mvin_cache_hit
    global mvin_cache_miss
    global mvout
    global compute
    mvin_cache_hit_all = {"spad": 16.6004183960958, "acc": 120.54796785426498, "mesh": 69.75608614542848}
    mvout_all = {"spad": 8.70642174885708, "acc": 112.25912905510398, "mesh": 64.86844914345728}
    compute_all = {"spad": 1.0646001865376011, "acc": 13.333890398976001, "mesh": 19.25971376896}

    mvin_cache_hit = (mvin_cache_hit_all["spad"] + 0 * mvin_cache_hit_all["acc"] + 0 * mvin_cache_hit_all["mesh"]) / (10 ** 4)
    mvin_cache_miss = 0
    mvout = (0 * mvout_all["spad"] + mvout_all["acc"] + 0 *  mvout_all["mesh"]) / (10 ** 4)
    compute = (compute_all["spad"] + compute_all["acc"] + compute_all["mesh"]) / (10 ** 3)

    # mvin_cache_hit = (mvin_cache_hit_all["spad"] + mvin_cache_hit_all["acc"] + mvin_cache_hit_all["mesh"]) / (10 ** 4)
    # mvin_cache_miss = 0
    # mvout = (mvout_all["spad"] + mvout_all["acc"] + mvout_all["mesh"]) / (10 ** 4)
    # compute = (compute_all["spad"] + compute_all["acc"] + compute_all["mesh"]) / (10 ** 3)

def analytical_model_pred(inst_count):
    return mvin_cache_hit * inst_count["mvin_cache_hit"] + mvin_cache_miss * inst_count["mvin_cache_miss"] + mvout * inst_count["mvout"] + compute * inst_count["compute"]


def get_inst_count(file_path):
    inst_count = {
        'mvin_cache_hit': 0,
        'mvin_cache_miss': 0,
        'mvout': 0,
        'compute': 0
    }

    with open(file_path, 'r') as file:
        for line in file:
            if "mvin" in line:
                inst_count["mvin_cache_hit"] += 1
            elif "mvout" in line:
                inst_count["mvout"] += 1
            elif "compute" in line:
                inst_count["compute"] += 1

    return inst_count


def get_energy(report_name='test', hammer_log_num='20240209-115959'):
    cycle_count = parse_hammer_output(hammer_log_num)

    filename = f"/scratch/jfwang/power-prediction/power-mappings-chipyard/vlsi/build/chipyard.harness.TestHarness.CustomGemminiSoCConfig-ChipTop/power-rtl-rundir/reports/{report_name}-baremetal-gemmini.hier.power.rpt"

    spad_dynamic_power = 0
    acc_dynamic_power = 0
    mesh_dynamic_power = 0

    gemmini_hierarchy = "/ChipTop/system/tile_prci_domain/tile_reset_domain_tile/gemmini/"
    gemmini_spad_modules = [gemmini_hierarchy + "spad/spad_mems_0/mem/mem_ext", gemmini_hierarchy + "spad/spad_mems_1/mem/mem_ext", gemmini_hierarchy + "spad/spad_mems_2/mem/mem_ext", gemmini_hierarchy + "spad/spad_mems_3/mem/mem_ext"]
    gemmini_acc_modules = [gemmini_hierarchy + "spad/acc_mems_0", gemmini_hierarchy + "spad/acc_mems_1"]
    gemmini_mesh_modules = [gemmini_hierarchy + "ex_controller/mesh"]

    with open(filename, 'r') as file:
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
        elif columns[7] in gemmini_mesh_modules:
            mesh_dynamic_power += dynamic_power

    spad_energy = spad_dynamic_power * (10 ** 3) * (cycle_count/frequency)
    acc_energy = acc_dynamic_power * (10 ** 3) * (cycle_count/frequency)
    mesh_energy = mesh_dynamic_power * (10 ** 3) * (cycle_count/frequency)

    return [spad_energy, acc_energy, mesh_energy]


def parse_hammer_output(hammer_log_num):
    directory_path = '/scratch/jfwang/power-prediction/power-mappings-chipyard/vlsi'
    file_name = f'hammer-vlsi-{hammer_log_num}.log'

    file_path = os.path.join(directory_path, file_name)
    with open(file_path, 'r') as file:
        file_content = file.read()
        phrase_pattern = r"Cycles taken: (\d+)"
        match = re.search(phrase_pattern, file_content)
        return int(match.group(1))

def convert_scientific_notation(value):
    try:
        return float(value)
    except ValueError:
        return value

workloads = {
    'matmul_test': ['/scratch/jfwang/power-prediction/matmul_test', '20240412-172036']
}

get_per_instruction_energy()

for key in workloads.keys():
    data = workloads[key]
    inst_count = get_inst_count(data[0])
    pred_energy = analytical_model_pred(inst_count)
    energy_data = get_energy(key, data[1])
    total_energy = sum(energy_data)

    print(key, "Actual Energy:")
    print("Total:", total_energy, "uJ")
    print("Breakdown:", energy_data)
    print("Predicted Energy:", pred_energy, "uJ")
    print("mvin_cache_hit:", mvin_cache_hit, "uJ")
    print("mvin_cache_miss:", mvin_cache_miss, "uJ")
    print("mvout:", mvout, "uJ")
    print("compute:", compute, "uJ")
    print("inst_count:", inst_count)
