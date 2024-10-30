# import os
# import re
# import random

# microbenchmark_dir = "templates/bareMetalC"
# # microbenchmark_list = [microbenchmark for microbenchmark in os.listdir(microbenchmark_dir) if re.match(r'^.*\.c$', microbenchmark) and microbenchmark != 'simple.c']
# microbenchmark_list = ['conv_1.c']

# define_pattern = re.compile(r'#define\s+(\w+)\s+(\d+)')
# sizes = {}

# for microbenchmark in microbenchmark_list:
#     with open(f"{microbenchmark_dir}/{microbenchmark}", "r") as file:
#         lines = file.readlines()

#     modified_lines = []
#     for line in lines:
#         if not re.search(r"// Matrix [\w\s]+ row", line.strip()):
#             match_message = define_pattern.match(line)
#             matrix_match = re.match(r"// Matrix (\w+), (\w+), (\w+) Setup", line.strip())
#             modified_lines.append(line)

#             if match_message:
#                 define_name = match_message.group(1)
#                 define_value = int(match_message.group(2))
#                 sizes[define_name] = define_value

#             elif matrix_match:
#                 matrix_name = matrix_match.group(1)
#                 matrix_rows = matrix_match.group(2)
#                 matrix_columns = matrix_match.group(3)

#                 matrix_rows_nums = sizes[matrix_rows]
#                 matrix_columns_nums = sizes[matrix_columns]

#                 modified_lines.append(f"\telem_t {matrix_name}[{matrix_rows}][{matrix_columns}] = {{ // Matrix {matrix_name} row\n")
        
#                 for i in range(matrix_rows_nums):
#                     row_line = "\t\t{"
#                     for j in range(matrix_columns_nums):
#                         rand_value = random.randint(-128, 127)
#                         row_line += str(rand_value)
#                         if j != matrix_columns_nums - 1:
#                             row_line += ", "
#                     row_line += "}"
#                     if i != matrix_rows_nums - 1:
#                         row_line += ","
#                     row_line += f" // Matrix {matrix_name} row\n"
#                     modified_lines.append(row_line)
#                 modified_lines.append(f"\t}}; // Matrix {matrix_name} row\n")
        
#     with open(f"{microbenchmark_dir}/{microbenchmark}", "w") as file:
#         file.writelines(modified_lines)



import os
import re
import random

microbenchmark_dir = "templates/bareMetalC"
microbenchmark_list = [microbenchmark for microbenchmark in os.listdir(microbenchmark_dir) if re.match(r'^.*\.c$', microbenchmark) and microbenchmark != 'simple.c']

define_pattern = re.compile(r'#define\s+(\w+)\s+(\d+)')
sizes = {}

for microbenchmark in microbenchmark_list:
    with open(f"{microbenchmark_dir}/{microbenchmark}", "r") as file:
        lines = file.readlines()

    modified_lines = []
    for line in lines:
        if not re.search(r"// Matrix [\w\s]+ row", line.strip()):
            match_message = define_pattern.match(line)
            matrix_match = re.match(r"// Matrix (\w+), ([^}]+) Setup", line.strip())
            modified_lines.append(line)

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

                    modified_lines.append(f"\telem_t {matrix_name}[{matrix_rows}][{matrix_columns}] = {{ // Matrix {matrix_name} row\n")

                    for i in range(matrix_rows_nums):
                        row_line = "\t\t{"
                        for j in range(matrix_columns_nums):
                            rand_value = random.randint(-128, 127)
                            row_line += str(rand_value)
                            if j != matrix_columns_nums - 1:
                                row_line += ", "
                        row_line += "}"
                        if i != matrix_rows_nums - 1:
                            row_line += ","
                        row_line += f" // Matrix {matrix_name} row\n"
                        modified_lines.append(row_line)

                elif len(matrix_dims) == 4:
                    matrix_dim_1, matrix_dim_2, matrix_dim_3, matrix_dim_4 = matrix_dims[0], matrix_dims[1], matrix_dims[2], matrix_dims[3]
                    matrix_dim_1_nums, matrix_dim_2_nums, matrix_dim_3_nums, matrix_dim_4_nums = matrix_dims_values[0], matrix_dims_values[1], matrix_dims_values[2], matrix_dims_values[3]

                    modified_lines.append(f"\telem_t {matrix_name}[{matrix_dim_1}][{matrix_dim_2}][{matrix_dim_3}][{matrix_dim_4}] = {{ // Matrix {matrix_name} row\n")

                    for i in range(matrix_dim_1_nums):
                        entry = "\t\t{" + f" // Matrix {matrix_name} row\n"
                        for j in range(matrix_dim_2_nums):
                            entry += "\t\t\t{" + f" // Matrix {matrix_name} row\n"
                            for k in range(matrix_dim_3_nums):
                                entry += "\t\t\t\t{"
                                for l in range(matrix_dim_4_nums):
                                    rand_value = random.randint(-128, 127)
                                    entry += str(rand_value)
                                    if l != matrix_dim_4_nums - 1:
                                        entry += ", "
                                entry += "}"
                                if k != matrix_dim_3_nums - 1:
                                    entry += ","
                                entry += f" // Matrix {matrix_name} row\n"
                            entry += "\t\t\t}"
                            if j != matrix_dim_2_nums - 1:
                                entry += ", "
                            entry += f" // Matrix {matrix_name} row\n"
                        entry += "\t\t}"
                        if i != matrix_dim_1_nums - 1:
                            entry += ", "
                        entry += f" // Matrix {matrix_name} row\n"
                        modified_lines.append(entry)
                
                modified_lines.append(f"\t}}; // Matrix {matrix_name} row\n")
        
    with open(f"{microbenchmark_dir}/{microbenchmark}", "w") as file:
        file.writelines(modified_lines)