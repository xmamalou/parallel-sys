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
optimized = {
    'dims'  : [],
    'jobs'  : [],
    'time'  : [],
}
unoptimized = {
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
data_array = data.split("[EXERCISE 5]")
for str in data_array:
    str = str.split("\n")
    if str != ['']:
        match str[1]:
            case 'type = optimized':
                optimized['jobs'].append(int(str[2].split(' = ')[1]))
                optimized['dims'].append(int(str[3].split(' = ')[1]))
                optimized['time'].append(float(str[4].split(' = ')[1]))
            case 'type = unoptimized':
                unoptimized['jobs'].append(int(str[2].split(' = ')[1]))
                unoptimized['dims'].append(int(str[3].split(' = ')[1]))
                unoptimized['time'].append(float(str[4].split(' = ')[1]))

# get the number of jobs and time for the 100000x10000 matrix
optimized_10000x10000 = {
    'jobs'  : optimized['jobs'][optimized['dims'].index(10000)],
    'time'  : optimized['time'][optimized['dims'].index(10000)],
}
optimized_df = pd.DataFrame(optimized_10000x10000)
# sort by jobs and separate the data into separate dataframes
optimized_df = optimized_df.sort_values(by='jobs')
optimized_dfs = [optimized_df.loc[optimized_df['jobs'] == i] for i in optimized_df['jobs'].unique()]
optimized_dfs = [df.reset_index(drop = True) for df in optimized_dfs]
optimized_dfs = [df.drop(columns = ['jobs']) for df in optimized_dfs]

unoptimized_10000x10000 = {
    'jobs'  : unoptimized['jobs'][unoptimized['dims'].index(10000)],
    'time'  : unoptimized['time'][unoptimized['dims'].index(10000)],
}
unoptimized_df = pd.DataFrame(unoptimized_10000x10000)
# sort by jobs and separate the data into separate dataframes
unoptimized_df = unoptimized_df.sort_values(by='jobs')
unoptimized_dfs = [unoptimized_df.loc[unoptimized_df['jobs'] == i] for i in unoptimized_df['jobs'].unique()]
unoptimized_dfs = [df.reset_index(drop = True) for df in unoptimized_dfs]
unoptimized_dfs = [df.drop(columns = ['jobs']) for df in unoptimized_dfs]

# create a histogram for the 10000x10000 matrix comparing the two algorithms for time
def plot_histogram(data1, data2, title, xlabel, ylabel):
    plt.hist(data1, bins=20, alpha=0.5, label='Optimized')
    plt.hist(data2, bins=20, alpha=0.5, label='Unoptimized')
    plt.title(title)
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.legend()
    plt.savefig("data/artifacts/plot5_" + title + ".png")
    plt.close()

# create a histogram for the 10000x10000 matrix comparing the two algorithms for time
plot_histogram(unoptimized_dfs[0]['time'], optimized_dfs[0]['time'], "Optimized vs Unoptimized", "Time (s)", "Frequency")
