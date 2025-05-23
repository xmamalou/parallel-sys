# Parallel Systems Exercise Batch 1 -- Solutions to Batch 1 of Exercises for the Parallel
# Systems Course of the "Computer Engineering" Masters Programme of NKUA
# Copyright (C) 2025 Christoforos-Marios Mamaloukas

# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import sys
import os
from enum import Enum

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

# READ FILE
data_filepath = os.getcwd() + "/" + sys.argv[1]
data_file = open(data_filepath)

data = data_file.read()

data_file.close()

# PARSE CONTENT
serial_pr = {
    'dims'  : [],
    'time'  : [],
}
serial_pc = {
    'dims'  : [],
    'time'  : [],
}
parallel_pr = {
    'dims'  : [],
    'jobs'  : [],
    'time'  : [],
}
parallel_pc = {
    'dims'  : [],
    'jobs'  : [],
    'time'  : [],
}

# The form of each entry in the output file is rather set.
# for serial data:
# - type
# - throws
# - π
# - time
# for parallel data:
# - type
# - jobs
# - throws
# - π
# - time
data_array = data.split("[EXERCISE 7]")
for str in data_array:
    str = str.split("\n")
    if str != ['']:
        match str[1]:
            case 'type = serial':
                if str[2] == 'algo = per column':
                    serial_pc['dims'].append(int(str[3].split(' = ')[1].split('x')[1]))
                    serial_pc['time'].append(float(str[5].split(' = ')[1]))
                if str[2] == 'algo = per row':
                    serial_pr['dims'].append(int(str[3].split(' = ')[1].split('x')[1]))
                    serial_pr['time'].append(float(str[5].split(' = ')[1]))
            case 'type = parallel':
                if str[2] == 'algo = per column':
                    parallel_pc['dims'].append(int(str[3].split(' = ')[1].split('x')[1]))
                    parallel_pc['jobs'].append(int(str[4].split(' = ')[1]))
                    parallel_pc['time'].append(float(str[5].split(' = ')[1]))
                if str[2] == 'algo = per row':
                    parallel_pr['dims'].append(int(str[3].split(' = ')[1].split('x')[1]))
                    parallel_pr['jobs'].append(int(str[4].split(' = ')[1]))
                    parallel_pr['time'].append(float(str[5].split(' = ')[1]))

# SERIAL PER ROW
serial_pr_df = pd.DataFrame(serial_pr)
serial_pr_df.sort_values(by = 'dims', ascending = True, inplace = True)

# SERIAL PER COLUMN
serial_pc_df = pd.DataFrame(serial_pc)
serial_pc_df.sort_values(by = 'dims', ascending = True, inplace = True)

# PARALLEL PER ROW
parallel_pr_df = pd.DataFrame(parallel_pr)
parallel_pr_df.sort_values(by = 'dims', ascending = True, inplace = True)
# we separate the dataframes by jobs, index is jobs-1
parallel_pr_dfs = parallel_pr_df.groupby('jobs')
parallel_pr_dfs = [parallel_pr_df.loc[parallel_pr_df['jobs'] == i] for i in parallel_pr_df['jobs'].unique()]
parallel_pr_dfs = [df.reset_index(drop = True) for df in parallel_pr_dfs]
parallel_pr_dfs = [df.drop(columns = ['jobs']) for df in parallel_pr_dfs]
parallel_pr_dfs = [df.sort_values(by = 'dims', ascending = True) for df in parallel_pr_dfs]

# PARALLEL PER COLUMN
parallel_pc_df = pd.DataFrame(parallel_pc)
parallel_pc_df.sort_values(by = 'dims', ascending = True, inplace = True)
# we separate the dataframes by jobs, index is jobs-1
parallel_pc_dfs = parallel_pc_df.groupby('jobs')
parallel_pc_dfs = [parallel_pc_df.loc[parallel_pc_df['jobs'] == i] for i in parallel_pc_df['jobs'].unique()]
parallel_pc_dfs = [df.reset_index(drop = True) for df in parallel_pc_dfs]
parallel_pc_dfs = [df.drop(columns = ['jobs']) for df in parallel_pc_dfs]
parallel_pc_dfs = [df.sort_values(by = 'dims', ascending = True) for df in parallel_pc_dfs]

# PLOTTING
# Plot the serial data on their own plot on the same figure
plt.figure(figsize = (10, 5))
plt.plot(serial_pr_df['dims'], serial_pr_df['time'], label = 'serial per row', marker = 'o')
plt.title("Serial (per row) Performance")
plt.xlabel("Matrix Size")
plt.ylabel("Time (msec)")
plt.xscale("log")
plt.xticks(serial_pr_df['dims'], [f"{serial_pr_df['dims'][i]}x{serial_pr_df['dims'][i]}" for i in range(len(serial_pr_df['dims']))])
plt.legend()
plt.grid()
plt.savefig("data/artifacts/plot7_serial_pr.png")
plt.close()

plt.figure(figsize = (10, 5))
plt.plot(serial_pc_df['dims'], serial_pc_df['time'], label = 'serial per column', marker = 'o')
plt.title("Serial (per column) Performance")
plt.xlabel("Matrix Size")
plt.ylabel("Time (msec)")
plt.xscale("log")
plt.xticks(serial_pc_df['dims'], [f"{serial_pc_df['dims'][i]}x{serial_pc_df['dims'][i]}" for i in range(len(serial_pc_df['dims']))])
plt.legend()
plt.grid()
plt.savefig("data/artifacts/plot7_serial_pc.png")
plt.close()

# Plot the parallel data on the same figure, per row is continuous line, per column is dashed
plt.figure(figsize = (10, 5))
for i in range(len(parallel_pr_dfs)):
    color = plt.cm.viridis(i / len(parallel_pr_dfs))
    plt.plot(parallel_pr_dfs[i]['dims'], parallel_pr_dfs[i]['time'], marker = 'o', color = color)
#add legend 
    plt.legend([f"jobs = {(i + 1)}" for i in range(len(parallel_pr_dfs))], loc = 'upper left')
plt.title("Parallel (per row) Performance")
plt.xlabel("Matrix Size")
plt.ylabel("Time (msec)")
plt.xscale("log")
plt.xticks(parallel_pr_dfs[0]['dims'], [f"{parallel_pr_dfs[0]['dims'][i]}x{parallel_pr_dfs[0]['dims'][i]}" for i in range(len(parallel_pr_dfs[0]['dims']))])
plt.grid()
plt.savefig("data/artifacts/plot7_parallel_pr.png")
plt.close()

plt.figure(figsize = (10, 5))
for i in range(len(parallel_pc_dfs)):
    color = plt.cm.viridis(i / len(parallel_pc_dfs))
    plt.plot(parallel_pc_dfs[i]['dims'], parallel_pc_dfs[i]['time'], marker = 'o', color = color)
#add legend
    plt.legend([f"jobs = {(i + 1)}" for i in range(len(parallel_pc_dfs))], loc = 'upper left')
plt.title("Parallel (per column) Performance")
plt.xlabel("Matrix Size")
plt.ylabel("Time (msec)")
plt.xscale("log")
plt.xticks(parallel_pc_dfs[0]['dims'], [f"{parallel_pc_dfs[0]['dims'][i]}x{parallel_pc_dfs[0]['dims'][i]}" for i in range(len(parallel_pc_dfs[0]['dims']))])
plt.grid()
plt.savefig("data/artifacts/plot7_parallel_pc.png")
plt.close()
