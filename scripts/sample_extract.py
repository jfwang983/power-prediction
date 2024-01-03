import sys
import pandas

def read_csv_to_dataframe():
    try:
        sys.argv[1]
    except Exception as e:
        raise ValueError("Not enough args")
    
    df = pandas.read_csv(input_file_path)
    df_sorted = df.sort_values(by='target.edp')

    if sys.argv[1] == "-regen":
        print("Regenerating dataset for binaries")
        df_sorted_samples = df_sorted.head(num_samples)
        write_csv(df_sorted_samples)
    elif sys.argv[1] == "-append":
        print("Appending to dataset for binaries")
        df_sorted_samples = pandas.read_csv(output_file_path)
        df_sorted_plus_new_samples = df_sorted.head(len(df_sorted_samples) + num_samples)
        write_csv(df_sorted_plus_new_samples)
    else:
        raise ValueError("Invalid arg")

def write_csv(df):
    try:
        df.to_csv(output_file_path, index=False, mode='w')
        print(f"Successfully written to CSV at {output_file_path}")
        df.to_csv(gemmini_output_file_path, index=False, mode='w')
        print(f"Successfully written to CSV at {gemmini_output_file_path}")
    except Exception as e:
        print(f"Error: {e}")

num_samples = 100
input_file_path = '../dosa/output_random/dataset.csv'
output_file_path = '../dosa/output_random/dataset_sorted.csv'
gemmini_output_file_path = '../power-mappings-chipyard/generators/gemmini/software/gemmini-rocc-tests/gemmini-data-collection/mappings/bert_large_test_random.csv'
read_csv_to_dataframe()