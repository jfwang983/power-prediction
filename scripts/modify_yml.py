import sys
import yaml

try:     
    sys.argv[1]
    sys.argv[2]
except Exception as e:
    raise ValueError("Not enough args")

workload = sys.argv[1]
report = sys.argv[2] + "-baremetal"

yaml_file = "../power-mappings-chipyard/vlsi/custom.yml"
waveform_path = f"output/chipyard.harness.TestHarness.CustomGemminiSoCConfig/{workload}-baremetal.fsdb"
report_types = ["gemmini", "spad_mems_0", "spad_mems_1", "spad_mems_2", "spad_mems_3", "acc_mems_0", "acc_mems_1", "mesh"]

with open(yaml_file, 'r') as f:
    custom = yaml.safe_load(f)
    report_configs = custom['power.inputs']['report_configs']
    for i in range(len(report_configs)):
        report_name = f"{report}-{report_types[i]}"
        report_configs[i]['waveform_path'] = waveform_path
        report_configs[i]['report_name'] = report_name

    with open(yaml_file, 'w',) as f:
        yaml.dump(custom, f, sort_keys=False)