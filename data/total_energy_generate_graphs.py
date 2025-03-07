import numpy as np
import pandas as pd
from sklearn import linear_model
import matplotlib.pyplot as plt
import scipy

df = pd.read_csv("workload_data.csv")

modules = ['spad', 'acc', 'mesh', 'gemmini']
inst_count = 'compute_count'
for module in modules:
    # Config
    plt.figure(figsize=(20, 15))
    plt.title(f'{module} Dynamic Energy vs. Compute Instruction Counts')
    plt.xlabel('Compute Instructions')
    plt.ylabel('Dynamic Energy (uJ)')

    # Data
    df_no_acc = df.loc[df['uses_accumulate'] == False]
    df_acc = df.loc[df['uses_accumulate'] == True & ~df['workload'].str.contains("conv", case=False, na=False)]
    df_conv = df.loc[df['workload'].str.contains("conv", case=False, na=False)]

    # Scatter plots
    plt.scatter(df_no_acc[inst_count], df_no_acc[f'{module}_energy'], label='No Accumulated Computes', color='r')
    plt.scatter(df_acc[inst_count], df_acc[f'{module}_energy'], label='Accumulated Computes', color='b')
    plt.scatter(df_conv[inst_count], df_conv[f'{module}_energy'], label='Convolution', color='g')

    for _, row in df.iterrows():
        x_coord = row[inst_count]
        y_coord = row[f'{module}_energy']
        label = f"{row['workload']}\n{row['input_matrix_dim']}"
        
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

    # Lines
    x, y = np.array(df_no_acc[inst_count], dtype=np.int64), np.array(df_no_acc[f'{module}_energy'], dtype=np.float64)
    a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
    r_squared = r_value ** 2
    plt.plot(x, a*x+b, label=f"{round(a, 5)}x+{round(b, 5)}, R^2={round(r_squared, 3)}", color='r')

    x, y = np.array(df_acc[inst_count], dtype=np.int64), np.array(df_acc[f'{module}_energy'], dtype=np.float64)
    a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
    r_squared = r_value ** 2
    plt.plot(x, a*x+b, label=f"{round(a, 5)}x+{round(b, 5)}, R^2={round(r_squared, 3)}", color='b')

    x, y = np.array(df_conv[inst_count], dtype=np.int64), np.array(df_conv[f'{module}_energy'], dtype=np.float64)
    a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
    r_squared = r_value ** 2
    plt.plot(x, a*x+b, label=f"{round(a, 5)}x+{round(b, 5)}, R^2={round(r_squared, 3)}", color='g')

    x, y = np.array(df[inst_count], dtype=np.int64), np.array(df[f'{module}_energy'], dtype=np.float64)
    a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
    r_squared = r_value ** 2
    plt.plot(x, a*x+b, label=f"{round(a, 5)}x+{round(b, 5)}, R^2={round(r_squared, 3)}", linewidth=3, color='gray')


    plt.xlim(left=0)
    plt.legend()

    plt.savefig(f'_new_plots3/{inst_count.removesuffix('_count')}_{module}_energy_plot.png')
    plt.close()