import sys
import yaml

try:     
    sys.argv[1]
    sys.argv[2]
except Exception as e:
    raise ValueError("Not enough args")

workload = sys.argv[1]
report = sys.argv[2]

yaml_file = "../power-mappings-chipyard/vlsi/custom.yml"
waveform_path = f"output/chipyard.harness.TestHarness.CustomGemminiSoCConfig/{workload}-baremetal.fsdb"
report_name = f"{report}-baremetal-gemmini"

with open(yaml_file, 'r') as f:
    custom = yaml.safe_load(f)
    custom['power.inputs']['report_configs'][0]['waveform_path'] = waveform_path
    custom['power.inputs']['report_configs'][0]['report_name'] = report_name

    with open(yaml_file, 'w',) as f:
        yaml.dump(custom, f, sort_keys=False)