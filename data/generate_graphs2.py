import numpy as np
import pandas as pd
from sklearn import linear_model
import matplotlib.pyplot as plt
import scipy

# csv_name = "software_data.csv"
# csv_name = "software_basic_data.csv"
# csv_name = "software_full_data.csv"
csv_name = "software_joules_data.csv"

# Dataframe
df = pd.read_csv(csv_name)

# Workload Split
df_workload = df.loc[~df['workload'].str.contains("microbenchmark", case=False, na=False)]
df_workload_sparse = df_workload.loc[~df_workload['workload'].str.contains("_test", case=False, na=False)]
df_workload_dense = df_workload.loc[df_workload['workload'].str.contains("_test", case=False, na=False)]

# Microbenchmark Split
df_microbenchmark = df.loc[df['workload'].str.contains("microbenchmark", case=False, na=False)]
df_compute = df_microbenchmark.loc[df_microbenchmark['workload'].str.contains("compute", case=False, na=False)]
df_compute_pre = df_compute.loc[df_compute['workload'].str.contains("preload", case=False, na=False)]
df_compute_acc = df_compute.loc[df_compute['workload'].str.contains("accumulated", case=False, na=False)]
df_mvin = df_microbenchmark.loc[df_microbenchmark['workload'].str.contains("mvin", case=False, na=False)]
df_mvout = df_microbenchmark.loc[df_microbenchmark['workload'].str.contains("mvout", case=False, na=False)]

# Sparse Workload
df_no_acc_sparse = df_workload_sparse.loc[df_workload_sparse['uses_accumulate'] == False]
df_acc_sparse = df_workload_sparse.loc[(df_workload_sparse['uses_accumulate'] == True) & (~df_workload_sparse['workload'].str.contains("conv", case=False, na=False))]
df_conv_sparse = df_workload_sparse.loc[df_workload_sparse['workload'].str.contains("conv", case=False, na=False)]

# Dense Workload
df_no_acc_dense = df_workload_dense.loc[df_workload_dense['uses_accumulate'] == False]
df_acc_dense = df_workload_dense.loc[(df_workload_dense['uses_accumulate'] == True) & (~df_workload_dense['workload'].str.contains("conv", case=False, na=False))]
df_conv_dense = df_workload_dense.loc[df_workload_dense['workload'].str.contains("conv", case=False, na=False)]

# Compute Preloaded Microbenchmark
df_compute_pre_B_col = df_compute_pre.loc[df_compute_pre['workload'].str.contains("preload_and_compute_microbenchmark_basic_B", case=False, na=False)]
df_compute_pre_A_row = df_compute_pre.loc[df_compute_pre['workload'].str.contains("preload_and_compute_microbenchmark_basic_A", case=False, na=False)]
df_compute_pre_outer = df_compute_pre.loc[df_compute_pre['workload'].str.contains("outer", case=False, na=False)]
df_compute_pre_inner = df_compute_pre.loc[df_compute_pre['workload'].str.contains("inner", case=False, na=False)]

# Compute Accumulated Microbenchmark
df_compute_acc_B_col = df_compute_acc.loc[df_compute_acc['workload'].str.contains("compute_accumulated_microbenchmark_basic_B", case=False, na=False)]
df_compute_acc_A_row = df_compute_acc.loc[df_compute_acc['workload'].str.contains("compute_accumulated_microbenchmark_basic_A", case=False, na=False)]
df_compute_acc_outer = df_compute_acc.loc[df_compute_acc['workload'].str.contains("outer", case=False, na=False)]
df_compute_acc_inner = df_compute_acc.loc[df_compute_acc['workload'].str.contains("inner", case=False, na=False)]

# Dataframe Info
legend = {
    "df_workload": {"df": df_workload, "label": "Sparse + Dense Workloads"},
    "df_workload_sparse": {"df": df_workload_sparse, "label": "Sparse Workloads"},
    "df_workload_dense": {"df": df_workload_dense, "label": "Dense Workloads"},

    "df_microbenchmark": {"df": df_microbenchmark, "label": "Microbenchmarks"},
    "df_compute": {"df": df_compute, "label": "Compute Microbenchmarks"},
    "df_compute_pre": {"df": df_compute_pre, "label": "Compute Preloaded"},
    "df_compute_acc": {"df": df_compute_acc, "label": "Compute Accumulated"},
    "df_mvin": {"df": df_mvin, "label": "Mvin"},
    "df_mvout": {"df": df_mvout, "label": "Mvout"},

    "df_no_acc_sparse": {"df": df_no_acc_sparse, "label": "No Accumulated Computes (Sparse)"},
    "df_acc_sparse": {"df": df_acc_sparse, "label": "Accumulated Computes (Sparse)"},
    "df_conv_sparse": {"df": df_conv_sparse, "label": "Convolution (Sparse)"},

    "df_no_acc_dense": {"df": df_no_acc_dense, "label": "No Accumulated Computes (Dense)"},
    "df_acc_dense": {"df": df_acc_dense, "label": "Accumulated Computes (Dense)"},
    "df_conv_dense": {"df": df_conv_dense, "label": "Convolution (Dense)"},

    "df_compute_pre_B_col": {"df": df_compute_pre_B_col, "label": "Compute Preloaded (16x16 * 16xB)"},
    "df_compute_pre_A_row": {"df": df_compute_pre_A_row, "label": "Compute Preloaded (Ax16 * 16x16)"},
    "df_compute_pre_outer": {"df": df_compute_pre_outer, "label": "Compute Preloaded (Ax16 * 16xB)"},
    "df_compute_pre_inner": {"df": df_compute_pre_inner, "label": "Compute Preloaded (16xY * Yx16)"},

    "df_compute_acc_B_col": {"df": df_compute_acc_B_col, "label": "Compute Accumulated (16x16 * 16xB)"},
    "df_compute_acc_A_row": {"df": df_compute_acc_A_row, "label": "Compute Accumulated (Ax16 * 16x16)"},
    "df_compute_acc_outer": {"df": df_compute_acc_outer, "label": "Compute Accumulated (Ax16 * 16xB)"},
    "df_compute_acc_inner": {"df": df_compute_acc_inner, "label": "Compute Accumulated (16xA * Ax16)"}
}

# Plot Info
workload_df_names = ["df_no_acc_sparse", "df_no_acc_dense", "df_acc_sparse", "df_acc_dense"]
# microbenchmark_df_names = ["df_compute_pre_B_col", "df_compute_pre_A_row", "df_compute_pre_outer", "df_compute_pre_inner"]
microbenchmark_df_names = ["df_compute_acc_B_col"]
combined_names = workload_df_names + microbenchmark_df_names
# combined_df = pd.concat([legend[df_name] for df_name in df_names], ignore_index=True)



modules = ['spad', 'acc', 'mesh', 'gemmini']
for module in modules:
    # Plot Axis
    x_axis = 'compute_count'
    y_axis = f'{module}_error'

    # Config
    plt.figure(figsize=(20, 15))
    plt.title(f'{module} Prediction Error vs. Compute Instructions')
    plt.xlabel('Compute Instructions')
    plt.ylabel('Prediction Error (%)')

    # All Scatter Plots
    for name in combined_names:
        target_df = legend[name]["df"]
        label = legend[name]["label"]
        plt.scatter(target_df[x_axis], target_df[y_axis], label=label)
    
    # # All Lines
    # for name in combined_names:
    #     target_df = legend[name]["df"]
    #     x, y = np.array(target_df[x_axis], dtype=np.float64), np.array(target_df[y_axis], dtype=np.float64)
    #     a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
    #     r_squared = r_value ** 2
    #     plt.plot(x, a*x+b, label=f"{round(a, 5)}x+{round(b, 5)}, R^2={round(r_squared, 3)}")

    # Workload Labels
    if len(workload_df_names) > 0:
        workload_combined_df = pd.concat([legend[workload_df_name]["df"] for workload_df_name in workload_df_names], ignore_index=True)
        for _, row in workload_combined_df.iterrows():
            x_coord = row[x_axis]
            y_coord = row[y_axis]
            # label = f"{row['workload']} - {round(row['mesh_utilization'], 2)}% util\n{row['input_matrix_dim']}\n{row['weight_matrices_dim']}"
            label = f"{row['workload']}\n{round(row['mesh_utilization'], 2)}% util"
            
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

    # Microbenchmark Labels
    if len(microbenchmark_df_names) > 0:
        microbenchmark_combined_df = pd.concat([legend[microbenchmark_df_name]["df"] for microbenchmark_df_name in microbenchmark_df_names], ignore_index=True)
        for _, row in microbenchmark_combined_df.iterrows():
            x_coord = row[x_axis]
            y_coord = row[y_axis]
            label = f"{row['input_matrix_dim']} x {row['weight_matrices_dim']}\n{round(row['mesh_utilization'], 2)}% util"
            
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


    plt.xlim(left=0)
    plt.legend()

    plt.savefig(f'generated_plots/{y_axis}_{x_axis}.png')
    plt.close()