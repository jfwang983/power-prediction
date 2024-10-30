import os
import sys
import pandas as pd
import matplotlib.pyplot as plt
plt.rcParams.update({'font.size': 20})

try:
    sys.argv[1]
except Exception as e:
    raise ValueError("Not enough args")

workload = sys.argv[1]

baseline_power = {"gemmini": 2.14618217913, "spad": 0.040646853904, "acc": 0.265598337549, "mesh": 0.812532273247}
title_dict = {"gemmini": "Gemmini", "spad": "Scratchpad", "acc": "Accumulator", "mesh": "PE Mesh"}

joules_reports_path = "../power-mappings-chipyard/vlsi/build/chipyard.harness.TestHarness.CustomGemminiSoCConfig-ChipTop/power-rtl-rundir/reports"
gemmini_report_names = [f"{joules_reports_path}/{workload}-baremetal-gemmini.profile.png.data"]
spad_report_names = [f"{joules_reports_path}/{workload}-baremetal-spad_mems_{i}.profile.png.data" for i in range(4)]
acc_report_names = [f"{joules_reports_path}/{workload}-baremetal-acc_mems_{i}.profile.png.data" for i in range(2)]
mesh_report_name = [f"{joules_reports_path}/{workload}-baremetal-mesh.profile.png.data"]

def generate_aggregated_power_plot(report_names, module_type):
    df = pd.DataFrame(columns=['sim_time', 'power'])

    for i in range(len(report_names)):
        row = 0
        with open(report_names[i]) as report:
            next(report)
            for line in report:
                data = line.rstrip('\n').split()
                data = [float(data_item) for data_item in data]
                try:
                    df.loc[row]['power'] += data[1]
                except KeyError:
                    df.loc[row] = data
                row += 1

    df['power'] = df.apply(lambda row: max(row['power'] - baseline_power[module_type], 0), axis=1)

    joules_data_dir_path = "../data/joules_output/" + workload

    if not os.path.exists(joules_data_dir_path):
        os.mkdir(joules_data_dir_path)

    df.to_csv(f"{joules_data_dir_path}/{module_type}_power_plot.csv", index=False)

    plt.figure(figsize=(15, 10))
    plt.plot(df['sim_time'], df['power'])
    plt.ylim(bottom=0)
    plt.xlim(left=0)
    plt.title(f"{title_dict[module_type]} Dynamic Power")
    plt.xlabel('Simulation Time (ns)')
    plt.ylabel('Dynamic Power (mW)')
    plt.savefig(f"{joules_data_dir_path}/{module_type}_power_plot.png")
    plt.close()


generate_aggregated_power_plot(gemmini_report_names, "gemmini")
generate_aggregated_power_plot(spad_report_names, "spad")
generate_aggregated_power_plot(acc_report_names, "acc")
generate_aggregated_power_plot(mesh_report_name, "mesh")