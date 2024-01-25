import re
import sys
import pandas

sys.path.insert(0, '..')
from dosa.energy_modeling import *

def sort_dataframe_sample():
    df = remove_duplicate_mappings()
    df_sorted = df.sort_values(by='target.edp')

    if flag == "-regen":
        print("Regenerating dataset for binaries")
        df_sorted_samples = df_sorted.head(num_samples)

        write_csv(df_sorted_samples)
    elif flag == "-append":
        print("Appending to dataset for binaries")
        df_sorted_samples = pandas.read_csv(output_file_path)
        df_sorted_plus_new_samples = df_sorted.head(len(df_sorted_samples) + num_samples)
        write_csv(df_sorted_plus_new_samples)
    else:
        raise ValueError("Invalid flag arg")


def remove_duplicate_mappings():
  updated_mapping_temp_filename = "../dosa/output_random/dataset_sorted_temp.csv"
  dosa_stats_dir = "../dosa/output_random"
  df = pandas.read_csv(input_file_path)
  df_fixed = fix_layer_mappings_matmul(df)

  try:
    df_fixed.to_csv(updated_mapping_temp_filename, index=False, mode='w')
    print(f"Successfully written to CSV at {updated_mapping_temp_filename}")
  except Exception as e:
    print(f"Error: {e}")

  dla_dataset = load_dataset(updated_mapping_temp_filename)
  _, _, _, timeloop_data = predict_energy(dosa_stats_dir, dla_dataset, True)
  df_timeloop_fixed = pandas.DataFrame.from_dict(timeloop_data)
  print(df_timeloop_fixed.head())
  timeloop_fixed_cols = ["target.cycle", "target.energy", "target.edp", "target.area"]
  for col in timeloop_fixed_cols:
    df_fixed[col] = df_timeloop_fixed[col]
  return df_fixed.drop_duplicates()

def fix_layer_mappings_matmul(df):
  for i in range(len(df)):
    mapping = parse_mapping(df.loc[i, "mapping.mapping"])

    # C Spatial Tiling Fix
    levels = ["L0", "L1", "L2", "L3"]
    spatial_pattern = re.compile(r'C(\d+)X')
    temporal_pattern = re.compile(r'C(\d+)')

    spatial_tile = {"index": 999, "size": 1}
    temporal_tiles = {}

    for item in mapping["L1"]:
      match = spatial_pattern.fullmatch(item)
      if match:
        spatial_tile = {"index": mapping["L1"].index(item), "size": int(match.group(1))}

    for level in levels:
      temporal_tile = {"index": 999, "size": 1}
      for item in mapping[level]:
        match = temporal_pattern.fullmatch(item)
        if match:
          temporal_tile = {"index": mapping[level].index(item), "size": int(match.group(1))}
      temporal_tiles[level] = temporal_tile

    index = 0
    total_product = spatial_tile["size"]
    for level in levels:
      total_product *= temporal_tiles[level]["size"]

    while spatial_tile["size"] < 16:
      if index < len(levels):
        level = levels[index]
        product = spatial_tile["size"] * temporal_tiles[level]["size"]
        if product > 16:
          spatial_tile["size"] = 16
          if product % 16 == 0:
            temporal_tiles[level]["size"] = product//16
          else:
            next_index = index + 1
            while temporal_tiles[levels[next_index]]["index"] == 999:
              next_index += 1
            next_level = levels[next_index]
            temporal_tiles[level]["size"] = 1
            temporal_tiles[next_level]["size"] = total_product//16
          break
        else:
          spatial_tile["size"] = product
          temporal_tiles[level]["size"] = 1
          if product == 16:
            break
          else:
            index += 1
      else:
        break

    if spatial_tile["index"] == 999:
      mapping["L1"].append("C" + str(spatial_tile["size"]) + "X")
    else:
      mapping["L1"][spatial_tile["index"]] = "C" + str(spatial_tile["size"]) + "X"

    for level in temporal_tiles:
      index = temporal_tiles[level]["index"]
      size = temporal_tiles[level]["size"]
      if index != 999:
        if size == 1:
          del mapping[level][index]
        else:
          mapping[level][index] = "C" + str(size)

    new_total_product = spatial_tile["size"]
    for level in levels:
      new_total_product *= temporal_tiles[level]["size"]
    try:
      assert total_product == new_total_product
    except AssertionError as e:
      print("Mapping Fix failed for C tiles")
      reconstructed_mapping = construct_mapping(mapping)
      print(df.loc[i, "mapping.mapping"])
      print(reconstructed_mapping)
      exit(1)

    # K Spatial Tiling Fix
    levels = ["L0", "L1", "L2", "L3"]
    spatial_pattern = re.compile(r'K(\d+)X')
    temporal_pattern = re.compile(r'K(\d+)')

    spatial_tile = {"index": 999, "size": 1}
    temporal_tiles = {}

    for item in mapping["L2"]:
      match = spatial_pattern.fullmatch(item)
      if match:
        spatial_tile = {"index": mapping["L2"].index(item), "size": int(match.group(1))}

    for level in levels:
      temporal_tile = {"index": 999, "size": 1}
      for item in mapping[level]:
        match = temporal_pattern.fullmatch(item)
        if match:
          temporal_tile = {"index": mapping[level].index(item), "size": int(match.group(1))}
      temporal_tiles[level] = temporal_tile

    index = 0
    total_product = spatial_tile["size"]
    for level in levels:
      total_product *= temporal_tiles[level]["size"]

    while spatial_tile["size"] < 16:
      if index < len(levels):
        level = levels[index]
        product = spatial_tile["size"] * temporal_tiles[level]["size"]
        if product > 16:
          spatial_tile["size"] = 16
          if product % 16 == 0:
            temporal_tiles[level]["size"] = product//16
          else:
            next_index = index + 1
            while temporal_tiles[levels[next_index]]["index"] == 999:
              next_index += 1
            next_level = levels[next_index]
            temporal_tiles[level]["size"] = 1
            temporal_tiles[next_level]["size"] = total_product//16
          break
        else:
          spatial_tile["size"] = product
          temporal_tiles[level]["size"] = 1
          if product == 16:
            break
          else:
            index += 1
      else:
        break
    
    if spatial_tile["index"] == 999:
      mapping["L2"].append("K" + str(spatial_tile["size"]) + "X")
    else:
      mapping["L2"][spatial_tile["index"]] = "K" + str(spatial_tile["size"]) + "X"

    for level in temporal_tiles:
      index = temporal_tiles[level]["index"]
      size = temporal_tiles[level]["size"]
      if index != 999:
        if size == 1:
          del mapping[level][index]
        else:
          mapping[level][index] = "K" + str(size)
    
    new_total_product = spatial_tile["size"]
    for level in levels:
      new_total_product *= temporal_tiles[level]["size"]
    try:
      assert total_product == new_total_product
    except AssertionError as e:
      print("Mapping Fix failed for K tiles")
      reconstructed_mapping = construct_mapping(mapping)
      print(df.loc[i, "mapping.mapping"])
      print(reconstructed_mapping)
      exit(1)
    
    # Fix mapping on row
    reconstructed_mapping = construct_mapping(mapping)
    if df.loc[i, "mapping.mapping"] != reconstructed_mapping:
      print("Mapping " + df.loc[i, "mapping.mapping"] + " adjusted to " + reconstructed_mapping)
    df.loc[i, "mapping.mapping"] = reconstructed_mapping

  return df

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

def construct_mapping(split_mapping):
  mapping = ""
  for key in split_mapping:
    mapping += key + split_mapping[key][0]
    for i in range(1, len(split_mapping[key])):
      mapping += " " + split_mapping[key][i]
    mapping += " - "
  return mapping[:-3]


def write_csv(df):
    try:
        df.to_csv(output_file_path, index=False, mode='w')
        print(f"Successfully written to CSV at {output_file_path}")
        df.to_csv(gemmini_output_file_path, index=False, mode='w')
        print(f"Successfully written to CSV at {gemmini_output_file_path}")
    except Exception as e:
        print(f"Error: {e}")

try:
    sys.argv[1]
    sys.argv[2]
except Exception as e:
    raise ValueError("Not enough args")

workload = sys.argv[1]
flag = sys.argv[2]

num_samples = 100
input_file_path = "../dosa/output_random/dataset.csv"
output_file_path = "../dosa/output_random/dataset_sorted.csv"
gemmini_output_file_path = f"../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/gemmini-data-collection/mappings/{workload}_random.csv"

sort_dataframe_sample()