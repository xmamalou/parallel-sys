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
serial_gol = {
    'jobs'  : [],
    'matrix': [],
    'generations': [],
    'time'  : [],
}
parallel_gol = {
    'jobs'  : [],
    'matrix': [],
    'generations': [],
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
data_array = data.split("[EXERCISE 6]")
for str in data_array:
    str = str.split("\n")
    if str != ['']:
        match str[1]:
            case 'type = serial':
                serial_gol['jobs'].append(int(str[2].split(' = ')[1]))
                serial_gol['matrix'].append(int(str[3].split(' = ')[1]))
                serial_gol['generations'].append(int(str[4].split(' = ')[1]))
                serial_gol['time'].append(float(str[5].split(' = ')[1]))
            case 'type = parallel':
                parallel_gol['jobs'].append(int(str[2].split(' = ')[1]))
                parallel_gol['matrix'].append(int(str[3].split(' = ')[1]))
                parallel_gol['generations'].append(int(str[4].split(' = ')[1]))
                parallel_gol['time'].append(float(str[5].split(' = ')[1]))

# SERIAL GAME OF LIFE
serial_gol_df = pd.DataFrame(serial_gol)
serial_gol_df = serial_gol_df.sort_values(by = 'matrix', ascending = True)

# PARALLEL GAME OF LIFE
parallel_gol_df = pd.DataFrame(parallel_gol)
parallel_gol_df = parallel_gol_df.sort_values(by = 'matrix', ascending = True)
# we separate the dataframes by jobs, index is jobs-1
parallel_gol_dfs = parallel_gol_df.groupby('jobs')
parallel_gol_dfs = [parallel_gol_df.loc[parallel_gol_df['jobs'] == i] for i in parallel_gol_df['jobs'].unique()]
parallel_gol_dfs = [df.reset_index(drop = True) for df in parallel_gol_dfs]
parallel_gol_dfs = [df.drop(columns = ['jobs']) for df in parallel_gol_dfs]
parallel_gol_dfs = [df.sort_values(by = 'matrix', ascending = True) for df in parallel_gol_dfs]

# PLOTTING
# Plot the locks data (time vs loops) on its own plot
plt.figure(figsize = (10, 5))
plt.title("Serial Game of Life")
plt.xscale("log")
plt.xlabel("Matrix dimensions")
plt.ylabel("Execution time (sec)")
color = plt.cm.viridis(len(serial_gol_df))
plt.plot(serial_gol_df['matrix'], serial_gol_df['time'] / 1000, marker = 'o', color = color)
#add legend
plt.xticks(serial_gol_df['matrix'], [f"{serial_gol_df['matrix'][i]}x{serial_gol_df['matrix'][i]}" for i in range(len(serial_gol_df['matrix']))])
plt.grid()
plt.savefig("data/artifacts/plot6_serial.png")
plt.close()

# Plot the atomic (time vs loops) on its own plot
plt.figure(figsize = (10, 5))
plt.title("Parallel Game of Life")
plt.xscale("log")
plt.xlabel("Matrix dimensions")
plt.ylabel("Execution time (sec)")
for i in range(len(parallel_gol_dfs)):
    color = plt.cm.viridis(i / len(parallel_gol_dfs))
    plt.plot(parallel_gol_dfs[i]['matrix'], parallel_gol_dfs[i]['time'] / 1000, marker = 'o', color = color)
#add legend
    plt.legend([f"jobs = {(i + 1)}" for i in range(len(parallel_gol_dfs))], loc = 'upper left')
plt.xticks(parallel_gol_dfs[0]['matrix'], [f"{parallel_gol_dfs[0]['matrix'][i]}x{parallel_gol_dfs[0]['matrix'][i]}" for i in range(len(parallel_gol_dfs[0]['matrix']))])
plt.grid()
plt.savefig("data/artifacts/plot6_parallel.png")
plt.close()


    
