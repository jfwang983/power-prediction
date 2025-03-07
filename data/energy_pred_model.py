import os
import re
import sys
import numpy as np
import pandas as pd
from sklearn.linear_model import LinearRegression

sys.path.append("../data")
import energy_extractor as ex

microbenchmarks = {
    "mvin_microbenchmark_basic": {"start_time": 17505.0, "end_time": 1696505.0},

    "mvin_microbenchmark_basic_1_col": {"start_time": 14505.0, "end_time": 2446505.0},
    "mvin_microbenchmark_basic_2_col": {"start_time": 13505.0, "end_time": 2445505.0},
    "mvin_microbenchmark_basic_4_col": {"start_time": 14505.0, "end_time": 2446505.0},
    "mvin_microbenchmark_basic_8_col": {"start_time": 15505.0, "end_time": 2054505.0},
    "mvin_microbenchmark_basic_16_col": {"start_time": 17505.0, "end_time": 1696505.0},

    "mvout_microbenchmark_basic": {"start_time": 27505.0, "end_time": 1921505.0},

    "mvout_microbenchmark_basic_1_col": {"start_time": 22505.0, "end_time": 2418505.0},
    "mvout_microbenchmark_basic_2_col": {"start_time": 24505.0, "end_time": 2466505.0},
    "mvout_microbenchmark_basic_4_col": {"start_time": 25505.0, "end_time": 2491505.0},
    "mvout_microbenchmark_basic_8_col": {"start_time": 25505.0, "end_time": 2188505.0},
    "mvout_microbenchmark_basic_16_col": {"start_time": 28505.0, "end_time": 1927505.0},

    "preload_and_compute_microbenchmark_basic": {"start_time": 25505.0, "end_time": 343505.0},

    # DIM x DIM by DIM x X
    "preload_and_compute_microbenchmark_basic_B_1_col": {"start_time": 23505.0, "end_time": 340505.0},
    "preload_and_compute_microbenchmark_basic_B_2_col": {"start_time": 22505.0, "end_time": 340505.0},
    "preload_and_compute_microbenchmark_basic_B_4_col": {"start_time": 23505.0, "end_time": 340505.0},
    "preload_and_compute_microbenchmark_basic_B_8_col": {"start_time": 23505.0, "end_time": 341505.0},
    "preload_and_compute_microbenchmark_basic_B_16_col": {"start_time": 25505.0, "end_time": 342505.0},

    # X x DIM by DIM x DIM,
    "preload_and_compute_microbenchmark_basic_A_1_row": {"start_time": 20505.0, "end_time": 338505.0},
    "preload_and_compute_microbenchmark_basic_A_2_row": {"start_time": 20505.0, "end_time": 338505.0},
    "preload_and_compute_microbenchmark_basic_A_4_row": {"start_time": 21505.0, "end_time": 339505.0},
    "preload_and_compute_microbenchmark_basic_A_8_row": {"start_time": 22505.0, "end_time": 340505.0},
    "preload_and_compute_microbenchmark_basic_A_16_row": {"start_time": 25505.0, "end_time": 342505.0},

    # 1 x DIM by DIM x X
    "preload_and_compute_microbenchmark_basic_1_outer": {"start_time": 17505.0, "end_time": 334505.0},
    "preload_and_compute_microbenchmark_basic_1_2_outer": {"start_time": 18505.0, "end_time": 335505.0},
    "preload_and_compute_microbenchmark_basic_1_4_outer": {"start_time": 19505.0, "end_time": 336505.0},
    "preload_and_compute_microbenchmark_basic_1_8_outer": {"start_time": 20505.0, "end_time": 337505.0},

    # 2 x DIM by DIM x X
    "preload_and_compute_microbenchmark_basic_2_1_outer": {"start_time": 18505.0, "end_time": 335505.0},
    "preload_and_compute_microbenchmark_basic_2_outer": {"start_time": 18505.0, "end_time": 335505.0},
    "preload_and_compute_microbenchmark_basic_2_4_outer": {"start_time": 19505.0, "end_time": 336505.0},
    "preload_and_compute_microbenchmark_basic_2_8_outer": {"start_time": 19505.0, "end_time": 337505.0},

    # 4 x DIM by DIM x X
    "preload_and_compute_microbenchmark_basic_4_1_outer": {"start_time": 19505.0, "end_time": 337505.0},
    "preload_and_compute_microbenchmark_basic_4_2_outer": {"start_time": 19505.0, "end_time": 336505.0},
    "preload_and_compute_microbenchmark_basic_4_outer": {"start_time": 20505.0, "end_time": 337505.0},
    "preload_and_compute_microbenchmark_basic_4_8_outer": {"start_time": 20505.0, "end_time": 337505.0},

    # 8 x DIM by DIM x X
    "preload_and_compute_microbenchmark_basic_8_1_outer": {"start_time": 20505.0, "end_time": 338505.0},
    "preload_and_compute_microbenchmark_basic_8_2_outer": {"start_time": 20505.0, "end_time": 337505.0},
    "preload_and_compute_microbenchmark_basic_8_4_outer": {"start_time": 21505.0, "end_time": 339505.0},
    "preload_and_compute_microbenchmark_basic_8_outer": {"start_time": 21505.0, "end_time": 339505.0},

    # DIM x DIM by DIM x DIM
    "preload_and_compute_microbenchmark_basic_16_outer": {"start_time": 25505.0, "end_time": 342505.0},

    # # DIM x X by X x DIM
    "preload_and_compute_microbenchmark_basic_1_inner": {"start_time": 16505.0, "end_time": 334505.0},
    "preload_and_compute_microbenchmark_basic_2_inner": {"start_time": 18505.0, "end_time": 335505.0},
    "preload_and_compute_microbenchmark_basic_4_inner": {"start_time": 19505.0, "end_time": 337505.0},
    "preload_and_compute_microbenchmark_basic_8_inner": {"start_time": 21505.0, "end_time": 338505.0},
    "preload_and_compute_microbenchmark_basic_16_inner": {"start_time": 25505.0, "end_time": 342505.0},

    "compute_accumulated_microbenchmark_basic": {"start_time": 24505.0, "end_time": 183505.0},

    # DIM x DIM by DIM x X
    "compute_accumulated_microbenchmark_basic_B_1_col": {"start_time": 23505.0, "end_time": 340505.0},
    "compute_accumulated_microbenchmark_basic_B_2_col": {"start_time": 23505.0, "end_time": 340505.0},
    "compute_accumulated_microbenchmark_basic_B_4_col": {"start_time": 23505.0, "end_time": 341505.0},
    "compute_accumulated_microbenchmark_basic_B_8_col": {"start_time": 24505.0, "end_time": 341505.0},
    "compute_accumulated_microbenchmark_basic_B_16_col": {"start_time": 25505.0, "end_time": 342505.0},

    # X x DIM by DIM x DIM,
    "compute_accumulated_microbenchmark_basic_A_1_row": {"start_time": 20505.0, "end_time": 338505.0},
    "compute_accumulated_microbenchmark_basic_A_2_row": {"start_time": 20505.0, "end_time": 338505.0},
    "compute_accumulated_microbenchmark_basic_A_4_row": {"start_time": 21505.0, "end_time": 339505.0},
    "compute_accumulated_microbenchmark_basic_A_8_row": {"start_time": 22505.0, "end_time": 340505.0},
    "compute_accumulated_microbenchmark_basic_A_16_row": {"start_time": 25505.0, "end_time": 343505.0},

    # 1 x DIM by DIM x X
    "compute_accumulated_microbenchmark_basic_1_outer": {"start_time": 17505.0, "end_time": 334505.0},
    "compute_accumulated_microbenchmark_basic_1_2_outer": {"start_time": 18505.0, "end_time": 335505.0},
    "compute_accumulated_microbenchmark_basic_1_4_outer": {"start_time": 19505.0, "end_time": 336505.0},
    "compute_accumulated_microbenchmark_basic_1_8_outer": {"start_time": 19505.0, "end_time": 337505.0},

    # 2 x DIM by DIM x X
    "compute_accumulated_microbenchmark_basic_2_1_outer": {"start_time": 18505.0, "end_time": 335505.0},
    "compute_accumulated_microbenchmark_basic_2_outer": {"start_time": 17505.0, "end_time": 335505.0},
    "compute_accumulated_microbenchmark_basic_2_4_outer": {"start_time": 18505.0, "end_time": 336505.0},
    "compute_accumulated_microbenchmark_basic_2_8_outer": {"start_time": 19505.0, "end_time": 337505.0},

    # 4 x DIM by DIM x X
    "compute_accumulated_microbenchmark_basic_4_1_outer": {"start_time": 19505.0, "end_time": 337505.0},
    "compute_accumulated_microbenchmark_basic_4_2_outer": {"start_time": 19505.0, "end_time": 336505.0},
    "compute_accumulated_microbenchmark_basic_4_outer": {"start_time": 19505.0, "end_time": 337505.0},
    "compute_accumulated_microbenchmark_basic_4_8_outer": {"start_time": 20505.0, "end_time": 337505.0},

    # 8 x DIM by DIM x X
    "compute_accumulated_microbenchmark_basic_8_1_outer": {"start_time": 20505.0, "end_time": 338505.0},
    "compute_accumulated_microbenchmark_basic_8_2_outer": {"start_time": 20505.0, "end_time": 337505.0},
    "compute_accumulated_microbenchmark_basic_8_4_outer": {"start_time": 21505.0, "end_time": 339505.0},
    "compute_accumulated_microbenchmark_basic_8_outer": {"start_time": 21505.0, "end_time": 339505.0},

    # DIM x DIM by DIM x DIM
    "compute_accumulated_microbenchmark_basic_16_outer": {"start_time": 25505.0, "end_time": 343505.0},

    # DIM x X by X x DIM
    "compute_accumulated_microbenchmark_basic_1_inner": {"start_time": 17505.0, "end_time": 334505.0},
    "compute_accumulated_microbenchmark_basic_2_inner": {"start_time": 17505.0, "end_time": 335505.0},
    "compute_accumulated_microbenchmark_basic_4_inner": {"start_time": 19505.0, "end_time": 337505.0},
    "compute_accumulated_microbenchmark_basic_8_inner": {"start_time": 21505.0, "end_time": 339505.0},
    "compute_accumulated_microbenchmark_basic_16_inner": {"start_time": 25505.0, "end_time": 343505.0}
}

workloads = [
    "matmul_1",
    "matmul_2",
    "matmul_3",
    "matmul_4",
    "matmul_5",
    "matmul_6",
    "matmul_7",
    "matmul_8",
    "matmul_9",
    "matmul_10",
    # "matmul_10_toggletest",
    # "matmul_10_toggletest2",
    "mlp_1",
    "mlp_2",
    "mlp_3",
    "mlp_4",
    "mlp_5",
    "mlp_6",
    "conv_1",
    "conv_2",
    "conv_3",
    "conv_4",
    "conv_5",
    "conv_6",
    "matmul_1_test",
    "matmul_2_test",
    "matmul_3_test",
    "matmul_4_test",
    "matmul_5_test",
    "matmul_6_test",
    "matmul_7_test",
    "matmul_8_test",
    "matmul_9_test",
    "matmul_10_test",
    "mlp_1_test",
    "mlp_2_test",
    "mlp_3_test",
    "mlp_4_test",
    "mlp_5_test",
    "mlp_6_test",
    "conv_1_test",
    "conv_2_test",
    "conv_3_test",
    "conv_4_test",
    "conv_5_test",
    "conv_6_test"
]

mvin_microbenchmarks = [
    "mvin_microbenchmark_basic_1_col",
    "mvin_microbenchmark_basic_2_col",
    "mvin_microbenchmark_basic_4_col",
    "mvin_microbenchmark_basic_8_col",
    "mvin_microbenchmark_basic_16_col"
]

mvout_microbenchmarks = [
    "mvout_microbenchmark_basic_1_col",
    "mvout_microbenchmark_basic_2_col",
    "mvout_microbenchmark_basic_4_col",
    "mvout_microbenchmark_basic_8_col",
    "mvout_microbenchmark_basic_16_col"
]

compute_preloaded_microbenchmarks = [
    # "preload_and_compute_microbenchmark_basic",

    # DIM x DIM by DIM x X
    "preload_and_compute_microbenchmark_basic_B_1_col",
    "preload_and_compute_microbenchmark_basic_B_2_col",
    "preload_and_compute_microbenchmark_basic_B_4_col",
    "preload_and_compute_microbenchmark_basic_B_8_col",
    "preload_and_compute_microbenchmark_basic_B_16_col",

    # X x DIM by DIM x DIM,
    "preload_and_compute_microbenchmark_basic_A_1_row",
    "preload_and_compute_microbenchmark_basic_A_2_row",
    "preload_and_compute_microbenchmark_basic_A_4_row",
    "preload_and_compute_microbenchmark_basic_A_8_row",
    # "preload_and_compute_microbenchmark_basic_A_16_row",

    # 1 x DIM by DIM x X
    "preload_and_compute_microbenchmark_basic_1_outer",
    "preload_and_compute_microbenchmark_basic_1_2_outer",
    "preload_and_compute_microbenchmark_basic_1_4_outer",
    "preload_and_compute_microbenchmark_basic_1_8_outer",

    # 2 x DIM by DIM x X
    "preload_and_compute_microbenchmark_basic_2_1_outer",
    "preload_and_compute_microbenchmark_basic_2_outer",
    "preload_and_compute_microbenchmark_basic_2_4_outer",
    "preload_and_compute_microbenchmark_basic_2_8_outer",

    # 4 x DIM by DIM x X
    "preload_and_compute_microbenchmark_basic_4_1_outer",
    "preload_and_compute_microbenchmark_basic_4_2_outer",
    "preload_and_compute_microbenchmark_basic_4_outer",
    "preload_and_compute_microbenchmark_basic_4_8_outer",

    # 8 x DIM by DIM x X
    "preload_and_compute_microbenchmark_basic_8_1_outer",
    "preload_and_compute_microbenchmark_basic_8_2_outer",
    "preload_and_compute_microbenchmark_basic_8_4_outer",
    "preload_and_compute_microbenchmark_basic_8_outer",

    # DIM x DIM by DIM x DIM
    # "preload_and_compute_microbenchmark_basic_16_outer",

    # DIM x X by X x DIM
    "preload_and_compute_microbenchmark_basic_1_inner",
    "preload_and_compute_microbenchmark_basic_2_inner",
    "preload_and_compute_microbenchmark_basic_4_inner",
    "preload_and_compute_microbenchmark_basic_8_inner",
    # "preload_and_compute_microbenchmark_basic_16_inner"
]

compute_accumulated_microbenchmarks = [
    # "compute_accumulated_microbenchmark_basic",

    # DIM x DIM by DIM x X
    "compute_accumulated_microbenchmark_basic_B_1_col",
    "compute_accumulated_microbenchmark_basic_B_2_col",
    "compute_accumulated_microbenchmark_basic_B_4_col",
    "compute_accumulated_microbenchmark_basic_B_8_col",
    "compute_accumulated_microbenchmark_basic_B_16_col",

    # X x DIM by DIM x DIM,
    "compute_accumulated_microbenchmark_basic_A_1_row",
    "compute_accumulated_microbenchmark_basic_A_2_row",
    "compute_accumulated_microbenchmark_basic_A_4_row",
    "compute_accumulated_microbenchmark_basic_A_8_row",
    # "compute_accumulated_microbenchmark_basic_A_16_row",

    # 1 x DIM by DIM x X
    "compute_accumulated_microbenchmark_basic_1_outer",
    "compute_accumulated_microbenchmark_basic_1_2_outer",
    "compute_accumulated_microbenchmark_basic_1_4_outer",
    "compute_accumulated_microbenchmark_basic_1_8_outer",

    # 2 x DIM by DIM x X
    "compute_accumulated_microbenchmark_basic_2_1_outer",
    "compute_accumulated_microbenchmark_basic_2_outer",
    "compute_accumulated_microbenchmark_basic_2_4_outer",
    "compute_accumulated_microbenchmark_basic_2_8_outer",

    # 4 x DIM by DIM x X
    "compute_accumulated_microbenchmark_basic_4_1_outer",
    "compute_accumulated_microbenchmark_basic_4_2_outer",
    "compute_accumulated_microbenchmark_basic_4_outer",
    "compute_accumulated_microbenchmark_basic_4_8_outer",

    # 8 x DIM by DIM x X
    "compute_accumulated_microbenchmark_basic_8_1_outer",
    "compute_accumulated_microbenchmark_basic_8_2_outer",
    "compute_accumulated_microbenchmark_basic_8_4_outer",
    "compute_accumulated_microbenchmark_basic_8_outer",

    # DIM x DIM by DIM x DIM
    # "compute_accumulated_microbenchmark_basic_16_outer",

    # DIM x X by X x DIM
    "compute_accumulated_microbenchmark_basic_1_inner",
    "compute_accumulated_microbenchmark_basic_2_inner",
    "compute_accumulated_microbenchmark_basic_4_inner",
    "compute_accumulated_microbenchmark_basic_8_inner",
    # "compute_accumulated_microbenchmark_basic_16_inner"
]

df_columns = [
    'workload',
    'input_matrix_dim',
    'weight_matrices_dim',
    'mesh_utilization',
    'mvin_count',
    'mvout_count',
    'compute_preloaded_count',
    'compute_accumulated_count',
    'compute_count',
    'inst_count',
    'uses_accumulate',
    'cycle_count',
    'plot_cycle_count',
    'gemmini_power',
    'spad_power',
    'acc_power',
    'mesh_power',
    'gemmini_energy',
    'spad_energy',
    'acc_energy',
    'mesh_energy',
    'pred_gemmini_energy',
    'pred_spad_energy',
    'pred_acc_energy',
    'pred_mesh_energy',
    'gemmini_error',
    'spad_error',
    'acc_error',
    'mesh_error'
]

modules = ["gemmini", "spad", "acc", "mesh"]

class EnergyModel:
    def __init__(self, epi_mode="ranged"):
        self.epi_mode = epi_mode
        self.energy_table = self.create_energy_table()
        self.mvin_energy_models = self.create_mvin_energy_models()
        self.mvout_energy_models = self.create_mvout_energy_models()
        self.compute_preloaded_energy_models = self.create_compute_preloaded_energy_models()
        self.compute_accumulated_energy_models = self.create_compute_accumulated_energy_models()
        # self.compute_accumulated_energy_models = self.create_compute_preloaded_energy_models()

    def create_energy_table(self):
        energy_table = {}
        for microbenchmark in microbenchmarks.keys():
            energy_table[microbenchmark] = self.get_microbenchmark_epi(microbenchmark)
        return energy_table

    def display_energy_table(self):
        for microbenchmark in microbenchmarks.keys():
            print(microbenchmark, self.energy_table[microbenchmark])

    def create_mvin_energy_models(self):
        matrix_dims = []
        for mvin_microbenchmark in mvin_microbenchmarks:
            inst_breakdown = self.get_inst_breakdown(mvin_microbenchmark)
            matrix_dim = list(list(inst_breakdown["mvin"].keys())[0])
            matrix_dims.append(matrix_dim)

        mvin_energy_models = {}
        for module in modules:
            epi_vals = [self.energy_table[mvin_microbenchmark][module] for mvin_microbenchmark in mvin_microbenchmarks]
            mvin_energy_model = LinearRegression()
            mvin_energy_model.fit(matrix_dims, epi_vals)
            mvin_energy_models[module] = mvin_energy_model
        return mvin_energy_models

    def create_mvout_energy_models(self):
        matrix_dims = []
        for mvout_microbenchmark in mvout_microbenchmarks:
            inst_breakdown = self.get_inst_breakdown(mvout_microbenchmark)
            matrix_dim = list(list(inst_breakdown["mvout"].keys())[0])
            matrix_dims.append(matrix_dim)

        mvout_energy_models = {}
        for module in modules:
            epi_vals = [self.energy_table[mvout_microbenchmark][module] for mvout_microbenchmark in mvout_microbenchmarks]
            mvout_energy_model = LinearRegression()
            mvout_energy_model.fit(matrix_dims, epi_vals)
            mvout_energy_models[module] = mvout_energy_model
        return mvout_energy_models

    def create_compute_preloaded_energy_models(self):
        matrix_dims = []
        for compute_preloaded_microbenchmark in compute_preloaded_microbenchmarks:
            inst_breakdown = self.get_inst_breakdown(compute_preloaded_microbenchmark)
            matrix_dim = list(list(inst_breakdown["compute_preloaded"].keys())[0])
            matrix_dims.append(matrix_dim)
        
        compute_preloaded_energy_models = {}
        for module in modules:
            epi_vals = [self.energy_table[compute_preloaded_microbenchmark][module] for compute_preloaded_microbenchmark in compute_preloaded_microbenchmarks]
            compute_preloaded_energy_model = LinearRegression()
            compute_preloaded_energy_model.fit(matrix_dims, epi_vals)
            compute_preloaded_energy_models[module] = compute_preloaded_energy_model
        return compute_preloaded_energy_models

    def create_compute_accumulated_energy_models(self):
        matrix_dims = []
        for compute_accumulated_microbenchmark in compute_accumulated_microbenchmarks:
            inst_breakdown = self.get_inst_breakdown(compute_accumulated_microbenchmark)
            matrix_dim = list(list(inst_breakdown["compute_accumulated"].keys())[0])
            matrix_dims.append(matrix_dim)
        
        compute_accumulated_energy_models = {}
        for module in modules:
            epi_vals = [self.energy_table[compute_accumulated_microbenchmark][module] for compute_accumulated_microbenchmark in compute_accumulated_microbenchmarks]
            compute_accumulated_energy_model = LinearRegression()
            compute_accumulated_energy_model.fit(matrix_dims, epi_vals)
            compute_accumulated_energy_models[module] = compute_accumulated_energy_model
        return compute_accumulated_energy_models

    def get_microbenchmark_epi(self, workload, iterations=1000):
        start_time, end_time = microbenchmarks[workload]["start_time"], microbenchmarks[workload]["end_time"]
        microbenchmark_data = ex.extract_data(workload, mode=self.epi_mode, start_time=start_time, end_time=end_time)
        epi = {module : microbenchmark_data["dynamic_energy"][module] / iterations for module in modules}
        return epi

    def get_inst_breakdown(self, workload):
        inst_count = {
            "mvin": {},
            "mvout": {},
            "compute_preloaded": {},
            "compute_accumulated": {}
        }

        spike_output = f"spike_output/{workload}_spike.log"
        with open(spike_output, 'r') as file:
            for line in file:
                key = None
                if "mvin" in line:
                    # Format: gemmini_extended_mvin#(dram_addr, sp_addr, cols, rows)
                    mvin_pattern = r"gemmini_extended_mvin(?:\d+)?\(\w+, 0x[0-9a-fA-F]+, (\d+), (\d+)\)"
                    mvin_pattern2 = r"gemmini_extended_mvin(?:\d+)?\(\w+\s*\+\s*0x[0-9a-fA-F]+, 0x[0-9a-fA-F]+, (\d+), (\d+)\)"
                    mvin_match = re.search(mvin_pattern, line)
                    mvin_match2 = re.search(mvin_pattern2, line)
                    if mvin_match:
                        cols, rows = int(mvin_match.group(1)), int(mvin_match.group(2))
                    else:
                        cols, rows = int(mvin_match2.group(1)), int(mvin_match2.group(2))
                    key, sub_key = "mvin", (rows, cols)

                elif "mvout" in line:
                    # Format: gemmini_extended_mvout(dram_addr, sp_addr, cols, rows)
                    mvout_pattern = r"gemmini_extended_mvout\(\w+, 0x[0-9a-fA-F]+, (\d+), (\d+)\)"
                    mvout_pattern2 = r"gemmini_extended_mvout\(\w+\s*\+\s*0x[0-9a-fA-F]+, 0x[0-9a-fA-F]+, (\d+), (\d+)\)"
                    mvout_match = re.search(mvout_pattern, line)
                    mvout_match2 = re.search(mvout_pattern2, line)
                    if mvout_match:
                        cols, rows = int(mvout_match.group(1)), int(mvout_match.group(2))
                    else:
                        cols, rows = int(mvout_match2.group(1)), int(mvout_match2.group(2))
                    key, sub_key = "mvout", (rows, cols)

                elif "gemmini_extended_preload" in line:
                    # Format: gemmini_extended_preload(B_sp_addr, C_sp_addr, B_cols, B_rows, C_cols, C_rows)
                    compute_pattern = r"gemmini_extended_preload\(0x[0-9a-fA-F]+, 0x[0-9a-fA-F]+, (\d+), (\d+), (\d+), (\d+)\)"
                    compute_match = re.search(compute_pattern, line)
                    B_cols, B_rows, C_cols, C_rows = int(compute_match.group(1)), int(compute_match.group(2)), int(compute_match.group(3)), int(compute_match.group(4))
                    A_cols, A_rows = B_rows, C_rows
                    sub_key = (A_rows, A_cols, B_cols)
                
                elif "compute_preloaded" in line:
                    # Format: gemmini_extended_compute_preloaded(A_sp_addr, D_sp_addr, A_cols, A_rows, B_cols, B_rows)
                    key = "compute_preloaded"

                elif "compute_accumulated" in line:
                    # Format: gemmini_extended_compute_accumulated(A_sp_addr, D_sp_addr, A_cols, A_rows, B_cols, B_rows)
                    key = "compute_accumulated"

                if key is not None:
                    if sub_key in inst_count[key]:
                        inst_count[key][sub_key] += 1
                    else:
                        inst_count[key][sub_key] = 1
                    sub_key = None

        return inst_count

    def get_matrices_dims(self, workload):
        microbenchmark_dir = f"../scripts/templates/bareMetalC"
        define_pattern = re.compile(r'#define\s+(\w+)\s+(\d+)')
        sizes = {}
        matrices = []

        with open(f"{microbenchmark_dir}/{workload}.c", "r") as file:
            lines = file.readlines()

        for line in lines:
            match_message = define_pattern.match(line)
            matrix_match = re.match(r"// Matrix (\w+), ([^}]+) Setup", line.strip())

            if match_message:
                define_name = match_message.group(1)
                define_value = int(match_message.group(2))
                sizes[define_name] = define_value

            elif matrix_match:
                matrix_name = matrix_match.group(1)
                matrix_dims = matrix_match.group(2).split(', ')
                matrix_dims_values = [sizes[matrix_dim] for matrix_dim in matrix_dims]

                if len(matrix_dims) == 2:
                    matrix_rows, matrix_columns = matrix_dims[0], matrix_dims[1]
                    matrix_rows_nums, matrix_columns_nums = matrix_dims_values[0], matrix_dims_values[1]
                    matrices.append(f"{matrix_rows_nums}x{matrix_columns_nums}")
                
                elif len(matrix_dims) == 4:
                    matrix_dim_1, matrix_dim_2, matrix_dim_3, matrix_dim_4 = matrix_dims[0], matrix_dims[1], matrix_dims[2], matrix_dims[3]
                    matrix_dim_1_nums, matrix_dim_2_nums, matrix_dim_3_nums, matrix_dim_4_nums = matrix_dims_values[0], matrix_dims_values[1], matrix_dims_values[2], matrix_dims_values[3]
                    matrices.append(f"{matrix_dim_1_nums}x{matrix_dim_2_nums}x{matrix_dim_3_nums}x{matrix_dim_4_nums}")
        
        return matrices

    def get_mesh_utilization(self, workload, inst_breakdown, cycle_count):
        max_utilization = 256 * cycle_count
        curr_utilization = 0
        for dims in inst_breakdown["compute_preloaded"]:
            A_rows, A_cols, B_cols = dims[0], dims[1], dims[2]
            curr_utilization += (A_rows * A_cols * B_cols * inst_breakdown["compute_preloaded"][dims])
        for dims in inst_breakdown["compute_accumulated"]:
            A_rows, A_cols, B_cols = dims[0], dims[1], dims[2]
            curr_utilization += (A_rows * A_cols * B_cols * inst_breakdown["compute_accumulated"][dims])
        mesh_utilization = (curr_utilization / max_utilization) * 100
        return mesh_utilization

    # matrix_dim: (row, col)
    # Assume row=DIM ALWAYS
    def predict_mvin_epi(self, matrix_dim):
        matrix_dim = np.array([list(matrix_dim)])
        return {module: float(self.mvin_energy_models[module].predict(matrix_dim)[0]) for module in modules}

    # matrix_dim: (row, col)
    # Assume row=DIM ALWAYS
    def predict_mvout_epi(self, matrix_dim):
        matrix_dim = np.array([list(matrix_dim)])
        return {module: float(self.mvout_energy_models[module].predict(matrix_dim)[0]) for module in modules}

    def predict_compute_preloaded_epi(self, matrix_dim):
        matrix_dim = np.array([list(matrix_dim)])
        return {module: float(self.compute_preloaded_energy_models[module].predict(matrix_dim)[0]) for module in modules}

    def predict_compute_accumulated_epi(self, matrix_dim):
        matrix_dim = np.array([list(matrix_dim)])
        return {module: float(self.compute_accumulated_energy_models[module].predict(matrix_dim)[0]) for module in modules}

    def predict_energy(self, workload):
        energy = {module : 0.0 for module in modules}
        inst_breakdown = self.get_inst_breakdown(workload)
        for inst in inst_breakdown.keys():
            for matrix_dim in inst_breakdown[inst].keys():
                if inst == "mvin":
                    epi = self.predict_mvin_epi(matrix_dim)
                elif inst == "mvout":
                    epi = self.predict_mvout_epi(matrix_dim)
                elif inst == "compute_preloaded":
                    epi = self.predict_compute_preloaded_epi(matrix_dim)
                elif inst == "compute_accumulated":
                    epi = self.predict_compute_accumulated_epi(matrix_dim)
                inst_count = inst_breakdown[inst][matrix_dim]
                energy = {module : energy[module] + epi[module] * inst_count for module in modules}
        return energy

    def predict_energy_basic(self, workload):
        energy = {module : 0.0 for module in modules}
        inst_breakdown = self.get_inst_breakdown(workload)
        for inst in inst_breakdown.keys():
            for matrix_dim in inst_breakdown[inst].keys():
                if inst == "mvin":
                    epi = self.energy_table["mvin_microbenchmark_basic"]
                elif inst == "mvout":
                    epi = self.energy_table["mvout_microbenchmark_basic"]
                elif inst == "compute_preloaded":
                    epi = self.energy_table["preload_and_compute_microbenchmark_basic"]
                elif inst == "compute_accumulated":
                    epi = self.energy_table["compute_accumulated_microbenchmark_basic"]
                inst_count = inst_breakdown[inst][matrix_dim]
                energy = {module : energy[module] + epi[module] * inst_count for module in modules}
        return energy

    def evaluate_prediction(self, workload, microbenchmark=False, basic=False, full=False, joules=False):
        pct_error = lambda actual, predicted : (predicted - actual)/actual * 100

        if basic:
            predicted_energy = self.predict_energy_basic(workload)            
        else:
            predicted_energy = self.predict_energy(workload)
        
        if full:
            extracted_data = ex.extract_data(workload, mode="full")
        elif joules:
            extracted_data = ex.extract_data(workload, mode="joules")
        else:
            extracted_data = ex.extract_data(workload)
        
        actual_power = {module : extracted_data["dynamic_power"][module] for module in modules}
        cycle_count, plot_cycle_count = extracted_data["cycle_count"], extracted_data["plot_cycle_count"]
        actual_energy = extracted_data["dynamic_energy"]

        if not microbenchmark:
            prediction_evaluation = {module : {"predicted_energy": predicted_energy[module], "actual_energy": actual_energy[module], "error": pct_error(actual_energy[module], predicted_energy[module])} for module in modules}
        else:
            prediction_evaluation = {module : {"predicted_energy": predicted_energy[module], "actual_energy": actual_energy[module], "error": 0} for module in modules}

        return actual_power, cycle_count, plot_cycle_count, prediction_evaluation

    def save_evaluation_data(self, workload, microbenchmark=False, basic=False, full=False, joules=False):
        # Matrix Dims
        matrix_dims = self.get_matrices_dims(workload)
        input_matrix_dim = matrix_dims[0]
        if microbenchmark and len(matrix_dims) > 1:
            weight_matrices_dim = matrix_dims[1]
        else:            
            weight_matrices_dim = matrix_dims[1:]
        # Instruction Counts
        inst_breakdown = self.get_inst_breakdown(workload)
        mvin_count = sum([inst_breakdown["mvin"][matrix_dim] for matrix_dim in inst_breakdown["mvin"]])
        mvout_count = sum([inst_breakdown["mvin"][matrix_dim] for matrix_dim in inst_breakdown["mvin"]])
        compute_preloaded_count = sum([inst_breakdown["compute_preloaded"][matrix_dim] for matrix_dim in inst_breakdown["compute_preloaded"]])
        compute_accumulated_count = sum([inst_breakdown["compute_accumulated"][matrix_dim] for matrix_dim in inst_breakdown["compute_accumulated"]])
        # Actual Power/Energy
        module_keys = ["predicted_energy", "actual_energy", "error"]
        actual_power, cycle_count, plot_cycle_count, prediction_evaluation = self.evaluate_prediction(workload, microbenchmark=microbenchmark, basic=basic, full=full, joules=joules)
        gemmini_power, spad_power, acc_power, mesh_power = [actual_power[module] for module in modules]
        pred_gemmini_energy, gemmini_energy, gemmini_error = (prediction_evaluation["gemmini"][key] for key in module_keys)
        pred_spad_energy, spad_energy, spad_error = (prediction_evaluation["spad"][key] for key in module_keys)
        pred_acc_energy, acc_energy, acc_error = (prediction_evaluation["acc"][key] for key in module_keys)
        pred_mesh_energy, mesh_energy, mesh_error = (prediction_evaluation["mesh"][key] for key in module_keys)
        # Utilization
        mesh_utilization = self.get_mesh_utilization(workload, inst_breakdown, cycle_count)

        df_row = pd.DataFrame([{
            'workload': workload,
            'input_matrix_dim': input_matrix_dim,
            'weight_matrices_dim': weight_matrices_dim,
            'mesh_utilization': mesh_utilization,
            'mvin_count': mvin_count,
            'mvout_count': mvout_count,
            'compute_preloaded_count': compute_preloaded_count,
            'compute_accumulated_count': compute_accumulated_count,
            'compute_count': compute_preloaded_count + compute_accumulated_count,
            'inst_count': mvin_count + mvout_count + compute_preloaded_count + compute_accumulated_count,
            'uses_accumulate': compute_accumulated_count != 0,
            'cycle_count': cycle_count,
            'plot_cycle_count': plot_cycle_count,
            'gemmini_power': gemmini_power,
            'spad_power': spad_power,
            'acc_power': acc_power,
            'mesh_power': mesh_power,
            'gemmini_energy': gemmini_energy,
            'spad_energy': spad_energy,
            'acc_energy': acc_energy,
            'mesh_energy': mesh_energy,
            'pred_gemmini_energy': pred_gemmini_energy,
            'pred_spad_energy': pred_spad_energy,
            'pred_acc_energy': pred_acc_energy,
            'pred_mesh_energy': pred_mesh_energy,
            'gemmini_error': gemmini_error,
            'spad_error': spad_error,
            'acc_error': acc_error,
            'mesh_error': mesh_error
        }])
        return df_row



model = EnergyModel()
df = pd.DataFrame(columns=df_columns)
for workload in workloads:
    new_row = model.save_evaluation_data(workload)
    df = pd.concat([df, new_row], ignore_index=True)
df.to_csv('workload_data.csv', index=False)
for microbenchmark in microbenchmarks:
    new_row = model.save_evaluation_data(microbenchmark, microbenchmark=True)
    df = pd.concat([df, new_row], ignore_index=True)
df.to_csv('software_data.csv', index=False)

df = pd.DataFrame(columns=df_columns)
for workload in workloads:
    new_row = model.save_evaluation_data(workload, basic=True)
    df = pd.concat([df, new_row], ignore_index=True)
df.to_csv('workload_basic_data.csv', index=False)
for microbenchmark in microbenchmarks:
    new_row = model.save_evaluation_data(microbenchmark, microbenchmark=True, basic=True)
    df = pd.concat([df, new_row], ignore_index=True)
df.to_csv('software_basic_data.csv', index=False)

model = EnergyModel(epi_mode="full")
df = pd.DataFrame(columns=df_columns)
for workload in workloads:
    new_row = model.save_evaluation_data(workload, full=True)
    df = pd.concat([df, new_row], ignore_index=True)
df.to_csv('workload_full_data.csv', index=False)
for microbenchmark in microbenchmarks:
    new_row = model.save_evaluation_data(microbenchmark, microbenchmark=True, full=True)
    df = pd.concat([df, new_row], ignore_index=True)
df.to_csv('software_full_data.csv', index=False)

model = EnergyModel(epi_mode="joules")
df = pd.DataFrame(columns=df_columns)
for workload in workloads:
    new_row = model.save_evaluation_data(workload, joules=True)
    df = pd.concat([df, new_row], ignore_index=True)
df.to_csv('workload_joules_data.csv', index=False)
for microbenchmark in microbenchmarks:
    new_row = model.save_evaluation_data(microbenchmark, microbenchmark=True, joules=True)
    df = pd.concat([df, new_row], ignore_index=True)
df.to_csv('software_joules_data.csv', index=False)

# model.display_energy_table()












# df = pd.DataFrame(columns=df_columns)
#         df = pd.concat([df, new_row], ignore_index=True)

# for workload in workload_data_table.keys():
#     if "mvin" not in workload and "mvout" not in workload and "preload_and_compute" not in workload and "compute_accumulated" not in workload:
#         inst_counts = list(workload_data_table[workload]["inst_count"].values())
#         input_matrix_dims = workload_data_table[workload]["matrices_dims"][0]
#         pred_spad_energy, pred_acc_energy, pred_mesh_energy, pred_gemmini_energy = predict_energy_micro_basic(inst_counts, energy_computation_type)
#         spad_energy, acc_energy, mesh_energy, gemmini_energy = workload_data_table[workload][energy_computation_type]["spad"], workload_data_table[workload][energy_computation_type]["acc"], workload_data_table[workload][energy_computation_type]["mesh"], workload_data_table[workload][energy_computation_type]["gemmini"]

#         spad_error = round(error_func(pred_spad_energy, spad_energy), 2)
#         acc_error = round(error_func(pred_acc_energy, acc_energy), 2)
#         mesh_error = round(error_func(pred_mesh_energy, mesh_energy), 2)
#         gemmini_error = round(error_func(pred_gemmini_energy, gemmini_energy), 2)

#         new_row = pd.DataFrame([{'Column1': 'value1', 'Column2': 'value2', 'Column3': 'value3'}])


#         new_row = pd.DataFrame([{
#             'workload': workload,
#             'input_matrix_dims': input_matrix_dims,
#             'mvin_count': inst_counts[0],
#             'mvout_count': inst_counts[1],
#             'preload_compute_count': inst_counts[2],
#             'accumulated_compute_count': inst_counts[3],
#             'compute_count': inst_counts[2] + inst_counts[3],
#             'uses_accumulated_computes': inst_counts[3] != 0,
#             'spad_energy': spad_energy,
#             'acc_energy': acc_energy,
#             'mesh_energy': mesh_energy,
#             'gemmini_energy': gemmini_energy,
#             'pred_spad_energy': pred_spad_energy,
#             'pred_acc_energy': pred_acc_energy,
#             'pred_mesh_energy': pred_mesh_energy,
#             'pred_gemmini_energy': pred_gemmini_energy,
#             'spad_error': spad_error,
#             'acc_error': acc_error,
#             'mesh_error': mesh_error,
#             'gemmini_error': gemmini_error
#         }])
#         df = pd.concat([df, new_row], ignore_index=True)

        # print(f"**{workload}**")
        # print(f"inst_counts: {workload_data_table[workload]["inst_count"]}")
        # print(f"matrices_dims: {workload_data_table[workload]["matrices_dims"]}")
        # # print(f"cycles: {workload_data_table[workload]["cycle_count"]}")
        # # print(f"spad_power:\t  {workload_data_table[workload]["avg_dynamic_power"]["spad"]},\tacc_power:\t {workload_data_table[workload]["avg_dynamic_power"]["acc"]},\tmesh_power:\t  {workload_data_table[workload]["avg_dynamic_power"]["mesh"]},\ttotal_power:\t   {workload_data_table[workload]["avg_dynamic_power"]["gemmini"]}")
        # print(f"spad_energy:\t  {spad_energy},\tacc_energy:\t {acc_energy},\tmesh_energy:\t  {mesh_energy},\tgemmini_energy:\t   {gemmini_energy}")
        # print(f"pred_spad_energy: {pred_spad_energy},\tpred_acc_energy: {pred_acc_energy},\tpred_mesh_energy: {pred_mesh_energy},\tpred_gemmini_energy: {pred_gemmini_energy}")
        # print(f"spad error: {spad_error}%,\t\t\tacc_error: {acc_error}%,\t\t\tmesh_error: {mesh_error}%,\t\t\tgemmini_error: {gemmini_error}%\n")

# df.to_csv('workloads2.csv', index=False)

# modules = ['spad', 'acc', 'mesh']
# for module in modules:
#     # Config
#     plt.figure(figsize=(30, 25))
#     plt.title(f'{module} Prediction Error vs. Compute Instruction Counts')
#     plt.xlabel('Compute Instructions')
#     plt.ylabel('Actual/Predicted')

#     # Data
#     df_no_acc = df.loc[df['uses_accumulated_computes'] == False]
#     df_acc = df.loc[df['uses_accumulated_computes'] == True & ~df['workload'].str.contains("conv", case=False, na=False)]
#     df_conv = df.loc[df['workload'].str.contains("conv", case=False, na=False)]

#     # Scatter plots
#     plt.scatter(df_no_acc['compute_count'], df_no_acc[f'{module}_error'], label='No Accumulated Computes', color='r')
#     plt.scatter(df_acc['compute_count'], df_acc[f'{module}_error'], label='Accumulated Computes', color='b')
#     plt.scatter(df_conv['compute_count'], df_conv[f'{module}_error'], label='Convolution', color='g')

#     for _, row in df.iterrows():
#         x_coord = row['compute_count']
#         y_coord = row[f'{module}_error']
#         label = f"{row['workload']}\n{row['input_matrix_dims']}"
        
#         offset = max(0.05 * abs(y_coord), 0.01)
        
#         # Annotate below the point
#         plt.annotate(
#             label, 
#             (x_coord, y_coord), 
#             textcoords="offset points", 
#             xytext=(0, -20),
#             ha='center', 
#             fontsize=8
#         )

#     # Lines
#     x, y = np.array(df_no_acc['compute_count'], dtype=np.int64), np.array(df_no_acc[f'{module}_error'], dtype=np.float64)
#     a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
#     r_squared = r_value ** 2
#     plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}", color='r')

#     x, y = np.array(df_acc['compute_count'], dtype=np.int64), np.array(df_acc[f'{module}_error'], dtype=np.float64)
#     a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
#     r_squared = r_value ** 2
#     plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}", color='b')

#     x, y = np.array(df_conv['compute_count'], dtype=np.int64), np.array(df_conv[f'{module}_error'], dtype=np.float64)
#     a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
#     r_squared = r_value ** 2
#     plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}", color='g')

#     x, y = np.array(df['compute_count'], dtype=np.int64), np.array(df[f'{module}_error'], dtype=np.float64)
#     a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
#     r_squared = r_value ** 2
#     plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}", linewidth=3, color='gray')

#     plt.xlim(left=0)
#     plt.legend()

#     plt.savefig(f'_test_plots2/compute_{module}_error_plot.png')
#     plt.close()




# # Config
# plt.figure(figsize=(15, 10))
# plt.title("Acc Prediction Error vs. Compute Instruction Counts")
# plt.xlabel('Compute Instructions')
# plt.ylabel('Error (%)')

# # Scatter plots
# plt.scatter(compute_values, acc_error_values, label='No Accumulated Computes')
# plt.scatter(compute_values2, acc_error_values2, label='Accumulated Computes')

# # Lines
# x, y = np.array(compute_values), np.array(acc_error_values)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# x, y = np.array(compute_values2), np.array(acc_error_values2)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# x, y = np.array(compute_values + compute_values2), np.array(acc_error_values + acc_error_values2)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# plt.xlim(left=0)
# # plt.ylim(bottom=0)
# plt.legend()

# plt.savefig(f"/bwrcq/scratch/jfwang983/power-prediction/data/_test_plots/compute_acc_error_plot.png")
# plt.close()



# # Config
# plt.figure(figsize=(15, 10))
# plt.title("Mesh Prediction Error vs. Compute Instruction Counts")
# plt.xlabel('Compute Instructions')
# plt.ylabel('Error (%)')

# # Scatter plots
# plt.scatter(compute_values, mesh_error_values, label='No Accumulated Computes')
# plt.scatter(compute_values2, mesh_error_values2, label='Accumulated Computes')

# # Lines
# x, y = np.array(compute_values), np.array(mesh_error_values)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# x, y = np.array(compute_values2), np.array(mesh_error_values2)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# x, y = np.array(compute_values + compute_values2), np.array(mesh_error_values + mesh_error_values2)
# a, b, r_value, p_value, std_err = scipy.stats.linregress(x, y)
# r_squared = r_value ** 2
# plt.plot(x, a*x+b, label=f"{a}x+{b}, R^2={round(r_squared, 2)}")

# plt.xlim(left=0)
# # plt.ylim(bottom=0)
# plt.legend()

# plt.savefig(f"/bwrcq/scratch/jfwang983/power-prediction/data/_test_plots/compute_mesh_error_plot.png")
# plt.close()



# microbenchmarks = [
#     "mvin_microbenchmark_basic",

#     "mvin_microbenchmark_basic_1_col",
#     "mvin_microbenchmark_basic_2_col",
#     "mvin_microbenchmark_basic_4_col",
#     "mvin_microbenchmark_basic_8_col",
#     "mvin_microbenchmark_basic_16_col",

#     "mvout_microbenchmark_basic",
    
#     "mvout_microbenchmark_basic_1_col",
#     "mvout_microbenchmark_basic_2_col",
#     "mvout_microbenchmark_basic_4_col",
#     "mvout_microbenchmark_basic_8_col",
#     "mvout_microbenchmark_basic_16_col"

#     "preload_and_compute_microbenchmark_basic",

#     # DIM x DIM by DIM x X
#     "preload_and_compute_microbenchmark_basic_B_1_col",
#     "preload_and_compute_microbenchmark_basic_B_2_col",
#     "preload_and_compute_microbenchmark_basic_B_4_col",
#     "preload_and_compute_microbenchmark_basic_B_8_col",
#     "preload_and_compute_microbenchmark_basic_B_16_col",

#     # X x DIM by DIM x DIM,
#     "preload_and_compute_microbenchmark_basic_A_1_row",
#     "preload_and_compute_microbenchmark_basic_A_2_row",
#     "preload_and_compute_microbenchmark_basic_A_4_row",
#     "preload_and_compute_microbenchmark_basic_A_8_row",
#     "preload_and_compute_microbenchmark_basic_A_16_row",

#     # 1 x DIM by DIM x X
#     "preload_and_compute_microbenchmark_basic_1_outer",
#     "preload_and_compute_microbenchmark_basic_1_2_outer",
#     "preload_and_compute_microbenchmark_basic_1_4_outer",
#     "preload_and_compute_microbenchmark_basic_1_8_outer",

#     # 2 x DIM by DIM x X
#     "preload_and_compute_microbenchmark_basic_2_1_outer",
#     "preload_and_compute_microbenchmark_basic_2_outer",
#     "preload_and_compute_microbenchmark_basic_2_4_outer",
#     "preload_and_compute_microbenchmark_basic_2_8_outer",

#     # 4 x DIM by DIM x X
#     "preload_and_compute_microbenchmark_basic_4_1_outer",
#     "preload_and_compute_microbenchmark_basic_4_2_outer",
#     "preload_and_compute_microbenchmark_basic_4_outer",
#     "preload_and_compute_microbenchmark_basic_4_8_outer",

#     # 8 x DIM by DIM x X
#     "preload_and_compute_microbenchmark_basic_8_1_outer",
#     "preload_and_compute_microbenchmark_basic_8_2_outer",
#     "preload_and_compute_microbenchmark_basic_8_4_outer",
#     "preload_and_compute_microbenchmark_basic_8_outer",

#     # DIM x DIM by DIM x DIM
#     "preload_and_compute_microbenchmark_basic_16_outer",

#     # DIM x X by X x DIM
#     "preload_and_compute_microbenchmark_basic_1_inner",
#     "preload_and_compute_microbenchmark_basic_2_inner",
#     "preload_and_compute_microbenchmark_basic_4_inner",
#     "preload_and_compute_microbenchmark_basic_8_inner",
#     "preload_and_compute_microbenchmark_basic_16_inner"
# ]