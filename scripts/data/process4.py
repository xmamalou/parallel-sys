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
pthread_barr = {
    'jobs'  : [],
    'loops': [],
    'time'  : [],
}
cond_var_barr = {
    'jobs'  : [],
    'loops': [],
    'time'  : [],
}
busy_wait_barr = {
    'jobs'  : [],
    'loops': [],
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
data_array = data.split("[EXERCISE 4]")
for str in data_array:
    str = str.split("\n")
    if str != ['']:
        match str[1]:
            case 'type = pthreads':
                pthread_barr['jobs'].append(int(str[2].split(' = ')[1]))
                pthread_barr['loops'].append(int(str[3].split(' = ')[1]))
                pthread_barr['time'].append(float(str[4].split(' = ')[1]))
            case 'type = custom with condition variable':
                cond_var_barr['jobs'].append(int(str[2].split(' = ')[1]))
                cond_var_barr['loops'].append(int(str[3].split(' = ')[1]))
                cond_var_barr['time'].append(float(str[4].split(' = ')[1]))
            case 'type = custom with busy waiting':
                busy_wait_barr['jobs'].append(int(str[2].split(' = ')[1]))
                busy_wait_barr['loops'].append(int(str[3].split(' = ')[1]))
                busy_wait_barr['time'].append(float(str[4].split(' = ')[1]))

pthread_barr_df = pd.DataFrame(pthread_barr)
pthread_barr_df = pthread_barr_df.sort_values(by = 'loops', ascending = True)
# we separate the dataframes by jobs, index is jobs-1
pthread_barr_dfs = pthread_barr_df.groupby('loops')
pthread_barr_dfs = [pthread_barr_df.loc[pthread_barr_df['loops'] == i] for i in pthread_barr_df['loops'].unique()]
pthread_barr_dfs = [df.reset_index(drop = True) for df in pthread_barr_dfs]
pthread_barr_dfs = [df.drop(columns = ['loops']) for df in pthread_barr_dfs]
pthread_barr_dfs = [df.sort_values(by = 'jobs', ascending = True) for df in pthread_barr_dfs]

# PTHREADS LOCKS
cond_var_barr_df = pd.DataFrame(cond_var_barr)
cond_var_barr_df = cond_var_barr_df.sort_values(by = 'loops', ascending = True)
# we separate the dataframes by jobs, index is jobs-1
cond_var_barr_dfs = cond_var_barr_df.groupby('loops')
cond_var_barr_dfs = [cond_var_barr_df.loc[cond_var_barr_df['loops'] == i] for i in cond_var_barr_df['loops'].unique()]
cond_var_barr_dfs = [df.reset_index(drop = True) for df in cond_var_barr_dfs]
cond_var_barr_dfs = [df.drop(columns = ['loops']) for df in cond_var_barr_dfs]
cond_var_barr_dfs = [df.sort_values(by = 'jobs', ascending = True) for df in cond_var_barr_dfs]

# PTHREADS NO SYNC
busy_wait_barr_df = pd.DataFrame(busy_wait_barr)
busy_wait_barr_df = busy_wait_barr_df.sort_values(by = 'loops', ascending = True)
# we separate the dataframes by jobs, index is jobs-1
busy_wait_barr_dfs = busy_wait_barr_df.groupby('loops')
busy_wait_barr_dfs = [busy_wait_barr_df.loc[busy_wait_barr_df['loops'] == i] for i in busy_wait_barr_df['loops'].unique()]
busy_wait_barr_dfs = [df.reset_index(drop = True) for df in busy_wait_barr_dfs]
busy_wait_barr_dfs = [df.drop(columns = ['loops']) for df in busy_wait_barr_dfs]
busy_wait_barr_dfs = [df.sort_values(by = 'jobs', ascending = True) for df in busy_wait_barr_dfs]

# PLOTTING
# Plot the locks data (time vs loops) on its own plot
plt.figure(figsize = (10, 5))
plt.title("Pthreads native barriers")
plt.xlabel("Number of jobs")
plt.ylabel("Execution time (sec)")
for i in range(len(pthread_barr_dfs)):
    if i == len(pthread_barr_dfs) - 1:
        continue
    color = plt.cm.viridis(i / len(pthread_barr_dfs))
    plt.plot(pthread_barr_dfs[i]['jobs'], pthread_barr_dfs[i]['time'] / 1000, marker = 'o', color = color)
#add legend
    plt.legend([f"loops = {10000*10**(i + 1)}" for i in range(len(pthread_barr_dfs))], loc = 'upper left')
plt.xticks(pthread_barr_dfs[0]['jobs'])
plt.grid()
plt.savefig("data/artifacts/plot4_pthreads.png")
plt.close()
# Plot the atomic (time vs loops) on its own plot
plt.figure(figsize = (10, 5))
plt.title("Custom barriers with condition variable")
plt.xlabel("Number of jobs")
plt.ylabel("Execution time (sec)")
for i in range(len(cond_var_barr_dfs)):
    if i == len(pthread_barr_dfs) - 1:
        continue
    color = plt.cm.viridis(i / len(cond_var_barr_dfs))
    plt.plot(cond_var_barr_dfs[i]['jobs'], cond_var_barr_dfs[i]['time']  / 1000, marker = 'o', color = color)
#add legend
    plt.legend([f"loops = {10000*10**(i + 1)}" for i in range(len(cond_var_barr_dfs))], loc = 'upper left')
plt.xticks(cond_var_barr_dfs[0]['jobs'])
plt.grid()
plt.savefig("data/artifacts/plot4_condvar.png")
plt.close()
# Plot the no sync (time vs loops) on its own plot
plt.figure(figsize = (10, 5))
plt.title("Custom barriers with busy waiting")
plt.xlabel("Number of jobs")
plt.ylabel("Execution time (sec)")
for i in range(len(busy_wait_barr_dfs)):
    if i == len(pthread_barr_dfs) - 1:
        continue
    color = plt.cm.viridis(i / len(busy_wait_barr_dfs))
    plt.plot(busy_wait_barr_dfs[i]['jobs'], busy_wait_barr_dfs[i]['time']  / 1000, marker = 'o', color = color)
#add legend
    plt.legend([f"loops = {10000*10**(i + 1)}" for i in range(len(busy_wait_barr_dfs))], loc = 'upper left')
plt.xticks(busy_wait_barr_dfs[0]['jobs'])
plt.grid()
plt.savefig("data/artifacts/plot4_busywait.png")
plt.close()

# Create a stackplot for each of the dataframes
plt.figure(figsize = (10, 5))
plt.title("Pthreads native barriers")
plt.xlabel("Number of jobs")
plt.ylabel("Execution time (msec)")
for i in reversed(range(len(pthread_barr_dfs))):
    if i == len(pthread_barr_dfs) - 1:
        continue
    color = plt.cm.viridis(i / len(pthread_barr_dfs))
    plt.stackplot(pthread_barr_dfs[i]['jobs'], pthread_barr_dfs[i]['time'], colors = [color])
#add legend
    plt.legend([f"loops = {10000*10**(i + 1)}" for i in range(len(pthread_barr_dfs))], loc = 'upper left')
plt.xticks(pthread_barr_dfs[0]['jobs'])
plt.grid()
plt.savefig("data/artifacts/plot4_pthreads_stack.png")
plt.close()

plt.figure(figsize = (10, 5))
plt.title("Custom barriers with condition variable")
plt.xlabel("Number of jobs")
plt.ylabel("Execution time (msec)")
for i in reversed(range(len(cond_var_barr_dfs))):
    if i == len(cond_var_barr_dfs) - 1:
        continue
    color = plt.cm.viridis(i / len(cond_var_barr_dfs))
    plt.stackplot(cond_var_barr_dfs[i]['jobs'], cond_var_barr_dfs[i]['time'], colors = [color])
#add legend
    plt.legend([f"loops = {10000*10**(i + 1)}" for i in range(len(cond_var_barr_dfs))], loc = 'upper left')
plt.xticks(cond_var_barr_dfs[0]['jobs'])
plt.grid()
plt.savefig("data/artifacts/plot4_condvar_stack.png")
plt.close()

plt.figure(figsize = (10, 5))
plt.title("Custom barriers with busy waiting")
plt.xlabel("Number of jobs")
plt.ylabel("Execution time (msec)")
for i in reversed(range(len(busy_wait_barr_dfs))):
    if i == len(busy_wait_barr_dfs) - 1:
        continue
    color = plt.cm.viridis(i / len(busy_wait_barr_dfs))
    plt.stackplot(busy_wait_barr_dfs[i]['jobs'], busy_wait_barr_dfs[i]['time'], colors = [color])
#add legend
    plt.legend([f"loops = {10000*10**(i + 1)}" for i in range(len(busy_wait_barr_dfs))], loc = 'upper left')
plt.xticks(busy_wait_barr_dfs[0]['jobs'])
plt.grid()
plt.savefig("data/artifacts/plot4_busywait_stack.png")
plt.close()


    
