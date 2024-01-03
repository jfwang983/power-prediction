import re
import os
import sys
import pandas
from dosa.energy_modeling import *

frequency = 62.5 * (10 ** 6)
process_scale_constant = 130/90

try:
  sys.argv[1] # dataset length
  sys.argv[2] # workload
  sys.argv[3] # workload prob yaml
except Exception as e:
  raise ValueError("Not enough args")

dataset_length = int(sys.argv[1])
prob = Prob(DATASET_ROOT_PATH / "workloads" / sys.argv[2] / sys.argv[3])
output_filepath = "dataset.csv"


def write_csv():
  df = generate_dataframe()
  try:
    df.to_csv(output_filepath, index=False, mode='w')
    print(f"Successfully written to CSV at {output_filepath}")
  except Exception as e:
    print(f"Error: {e}")


def generate_dataframe():
  df_timeloop_dosa = parse_dosa_output()
  vcs_gemmini_cycle_counts = parse_hammer_output()
  df_timeloop_dosa["vcs.cycle"] = [int(vcs_gemmini_cycle_count) for vcs_gemmini_cycle_count in vcs_gemmini_cycle_counts]

  df_joules = parse_joules_output(0)
  for i in range(1, dataset_length):
    df_joules = df_joules.append(parse_joules_output(i), ignore_index=True)

  df = df_timeloop_dosa.join(df_joules, how="left")
  df["joules.spad_energy"] = (df["joules.spad_dynamic_power"] * (10 ** 3)) * (df["vcs.cycle"]/frequency)
  df["joules.acc_energy"] = (df["joules.acc_dynamic_power"] * (10 ** 3)) * (df["vcs.cycle"]/frequency)
  df["joules.mesh_energy"] = (df["joules.mesh_dynamic_power"] * (10 ** 3)) * (df["vcs.cycle"]/frequency)
  df["joules.energy"] = (df["joules.dynamic_power"] * (10 ** 3)) * (df["vcs.cycle"]/frequency)
  df["dosa.dynamic_power"] = (process_scale_constant ** 2) * (df["dosa.energy_no_dram"] * (10 ** -3)) / (df["target.cycle"]/frequency)

  df['mapping.parsed_mapping'] = [mapping_utils.process_mapping(df.loc[i, 'mapping.mapping'], prob.shape).tolist() for i in range(len(df))]
  def create_new_columns(row):
    for i, item in enumerate(row['mapping.parsed_mapping']):
        row[f'mapping.{i}'] = item
    return row

  df = df.apply(create_new_columns, axis=1)
  df = df.drop(columns=["mapping.parsed_mapping"])
  return df


def parse_dosa_output():
  updated_mapping_temp_filename = "../dosa/output_random/dataset_sorted_temp.csv"
  timeloop_filename = "../dosa/output_random/dataset_sorted.csv"
  dosa_stats_dir = "../dosa/output_random"

  df_timeloop = pandas.read_csv(timeloop_filename).head(dataset_length)
  df_timeloop = fix_layer_mappings(df_timeloop)

  try:
    df_timeloop.to_csv(updated_mapping_temp_filename, index=False, mode='w')
    print(f"Successfully written to CSV at {updated_mapping_temp_filename}")
  except Exception as e:
    print(f"Error: {e}")

  dla_dataset = load_dataset(updated_mapping_temp_filename)
  pred, accesses, ruw_accesses, timeloop_data = predict_energy(dosa_stats_dir, dla_dataset, True)

  df_timeloop_fixed = pandas.DataFrame.from_dict(timeloop_data)

  df_total_accesses = pandas.DataFrame(accesses.numpy()).head(dataset_length)
  df_total_accesses = df_total_accesses.rename(columns={0: "dosa.pe_accesses", 1: "dosa.register_accesses", 2: "dosa.acc_accesses", 3: "dosa.spad_accesses", 4: "dosa.dram_accesses"}, errors="raise")  

  df_ruw_accesses = pandas.DataFrame(ruw_accesses.numpy())
  ruw_accesses_columns = {}
  mem_lvl = ["pe", "register", "acc", "spad", "dram"]
  access_types = ["read", "update", "write"]
  index = 0
  for i in range(len(mem_lvl)):
    for j in range(len(access_types)):
      ruw_accesses_columns[index] = "dosa." + mem_lvl[i] + "_" + access_types[j] + "_accesses"
      index += 1
  df_ruw_accesses = df_ruw_accesses.rename(columns=ruw_accesses_columns, errors="raise")

  df_accesses = df_ruw_accesses.join(df_total_accesses, how="left")
  
  df_energy = pandas.DataFrame(pred.numpy()).head(dataset_length)
  df_energy = df_energy.rename(columns={0: "dosa.pe_energy", 1: "dosa.register_energy", 2: "dosa.acc_energy", 3: "dosa.spad_energy", 4: "dosa.dram_energy"}, errors="raise")
  df_energy["dosa.energy_no_dram"] = df_energy["dosa.pe_energy"] + df_energy["dosa.register_energy"] + df_energy["dosa.acc_energy"] + df_energy["dosa.spad_energy"]

  df_timeloop = df_timeloop.drop(columns=["target.cycle", "target.energy", "target.edp", "target.area"])
  df_timeloop = df_timeloop_fixed.join(df_timeloop, how="left")
  df_dosa = df_accesses.join(df_energy, how="left")
  df_combined = df_timeloop.join(df_dosa, how="left")
  return df_combined


def fix_layer_mappings(df_timeloop):
  for i in range(len(df_timeloop)):
    mapping = parse_mapping(df_timeloop.loc[i, "mapping.mapping"])

    # C Spatial Tiling Fix
    spatial_pattern = re.compile(r'C(\d+)X')
    temporal_pattern = re.compile(r'C(\d+)')
    for item in mapping["L1"]:
      match = temporal_pattern.fullmatch(item)
      if match:
        temporal_tile_size = int(match.group(1))
        temporal_match_index = mapping["L1"].index(item)
        break
      else:
        temporal_tile_size = 1
        temporal_match_index = 999
    for item in mapping["L1"]:
      match = spatial_pattern.fullmatch(item)
      if match:
        spatial_tile_size = int(match.group(1))
        spatial_match_index = mapping["L1"].index(item)
        break
      else:
        spatial_tile_size = 1
        spatial_match_index = 999
    product = spatial_tile_size * temporal_tile_size
    if spatial_tile_size < 16:
      if product > 16:
        new_spatial_tile_size = "C16X"
        new_temporal_tile_size = "C" + str(int(product/16))
        if spatial_match_index == 999:
          mapping["L1"].append(new_spatial_tile_size)
        else:
          mapping["L1"][spatial_match_index] = new_spatial_tile_size
        mapping["L1"][temporal_match_index] = new_temporal_tile_size
      elif product > 1:
          new_spatial_tile_size = "C" + str(int(product)) + "X"
          if spatial_match_index == 999:
            mapping["L1"].append(new_spatial_tile_size)
          else:
            mapping["L1"][spatial_match_index] = new_spatial_tile_size
          if temporal_match_index != 999:
            del mapping["L1"][temporal_match_index]
    
    # K Spatial Tiling Fix
    spatial_pattern = re.compile(r'K(\d+)X')
    temporal_pattern = re.compile(r'K(\d+)')
    for item in mapping["L1"]:
      match = temporal_pattern.fullmatch(item)
      if match:
        temporal_tile_size = int(match.group(1))
        temporal_match_index = mapping["L1"].index(item)
        break
      else:
        temporal_tile_size = 1
        temporal_match_index = 999
    for item in mapping["L2"]:
      match = spatial_pattern.fullmatch(item)
      if match:
        spatial_tile_size = int(match.group(1))
        spatial_match_index = mapping["L2"].index(item)
        break
      else:
        spatial_tile_size = 1
        spatial_match_index = 999
    product = spatial_tile_size * temporal_tile_size
    if spatial_tile_size < 16:
      if product > 16:
        new_spatial_tile_size = "K16X"
        new_temporal_tile_size = "K" + str(int(product/16))
        if spatial_match_index == 999:
          mapping["L2"].append(new_spatial_tile_size)
        else:
          mapping["L2"][spatial_match_index] = new_spatial_tile_size
        mapping["L1"][temporal_match_index] = new_temporal_tile_size
      elif product > 1:
        new_spatial_tile_size = "K" + str(int(product)) + "X"
        if spatial_match_index == 999:
          mapping["L2"].append(new_spatial_tile_size)
        else:
          mapping["L2"][spatial_match_index] = new_spatial_tile_size
        if temporal_match_index != 999:
          del mapping["L1"][temporal_match_index]
    
    # Fix mapping on row
    reconstructed_mapping = ' - '.join([f"{identifier}{' '.join(entries)}" for identifier, entries in mapping.items()])
    df_timeloop.loc[i, "mapping.mapping"] = reconstructed_mapping

  return df_timeloop


def parse_mapping(mapping):
  # Split by "-"
  first_split = mapping.split(" - ")
  result = {}

  # Split each part further by " "
  for part in first_split:
    match = re.match(r'([A-Za-z]+\d+)', part)  # Match the identifier (e.g., L1)
    if match:
      identifier = match.group()
      entries = part.replace(match.group(), '').split(" ")
      result[identifier] = entries
  
  return result


def parse_hammer_output():
  directory_path = './power-mappings-chipyard/vlsi'
  prefix = 'hammer-vlsi'

  all_files = sorted([f for f in os.listdir(directory_path) if f.startswith(prefix)])
  starting_file = 'hammer-vlsi-20231230-091702.log' # Will need to change
  start_index = all_files.index(starting_file) if starting_file in all_files else 0
  matching_files = all_files[start_index:]

  file_number = 0
  keyword = f"chipyard.harness.TestHarness.CustomGemminiSoCConfig/matmul_tilings_{file_number}-baremetal.fsdb"
  cycle_counts = []

  for file_name in matching_files:
    file_path = os.path.join(directory_path, file_name)
    with open(file_path, 'r') as file:
      file_content = file.read()
      if keyword in file_content:
        phrase_pattern = r"Gemmini tiled matmul took (\d+) cycles"
        match = re.search(phrase_pattern, file_content)
        if match:
          cycles_value = match.group(1)  # Extract the value within the parentheses
          cycle_counts.append(cycles_value)
          file_number += 1
          keyword = f"chipyard.harness.TestHarness.CustomGemminiSoCConfig/matmul_tilings_{file_number}-baremetal.fsdb"
  return cycle_counts[:dataset_length]

def parse_joules_output(report_number):
  # Joules report format
  filename = f"./power-mappings-chipyard/vlsi/build/chipyard.harness.TestHarness.CustomGemminiSoCConfig-ChipTop/power-rtl-rundir/reports/bert-inst_gemmini-{report_number}.power.rpt"

  # Get file contents
  file = open(filename, 'r')
  content = file.read()
  file.close() 

  # Define the regular expression pattern
  category_pattern = re.compile(r'(\w+)\s+([\d.]+[eE][+\-]?\d+)\s+([\d.]+[eE][+\-]?\d+)\s+([\d.]+[eE][+\-]?\d+)\s+([\d.]+[eE][+\-]?\d+)\s+([\d.]+)%')

  # Create a dictionary to store category information
  category_dict = {}

  # Find and store category information in the dictionary
  for match in category_pattern.finditer(content):
    category, leakage, internal, switching, total, row_percentage = match.groups()
    category_dict[category.lower()] = {"leakage": convert_scientific_notation(leakage), "internal": convert_scientific_notation(internal), "switching": convert_scientific_notation(switching)}

  # Convert the dictionary to a pandas DataFrame
  df = pandas.DataFrame(category_dict).T

  # Convert DataFrame into a single row
  types = ["leakage", "internal", "switching"]
  components = ["memory", "register", "latch", "logic", "bbox", "clock", "pad", "pm", "subtotal"]
  columns = [f'joules.{component}_{type}' for component in components for type in types] + ['joules.dynamic_power']
  data_row = [[df.iloc[i][j] for i in range(len(df)) for j in range(len(df.columns))] + [df.iloc[8][1] + df.iloc[8][2]]]
  df_row = pandas.DataFrame(data_row, columns=columns)

  # Joules hierarchy report format
  filename = f"./power-mappings-chipyard/vlsi/build/chipyard.harness.TestHarness.CustomGemminiSoCConfig-ChipTop/power-rtl-rundir/reports/bert-inst_gemmini-{report_number}.hier.power.rpt"

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
  
  df_row["joules.spad_dynamic_power"] = spad_dynamic_power
  df_row["joules.acc_dynamic_power"] = acc_dynamic_power
  df_row["joules.mesh_dynamic_power"] = mesh_dynamic_power
  return df_row

def convert_scientific_notation(value):
  try:
    return float(value)
  except ValueError:
    # If the conversion fails, return the original value
    return value

def get_files_starting_with(directory_path, prefix):
  matching_files = [f for f in os.listdir(directory_path) if f.startswith(prefix)]
  return matching_files


write_csv()
# add_columns()