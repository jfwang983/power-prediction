import sys
import pandas

def read_csv_to_dataframe():
    df = pandas.read_csv(input_file_path)
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
input_file_path = '../dosa/output_random/dataset.csv'
output_file_path = '../dosa/output_random/dataset_sorted.csv'
gemmini_output_file_path = f"../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/gemmini-data-collection/mappings/{workload}_random.csv"
read_csv_to_dataframe()