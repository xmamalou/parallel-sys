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
pthread_locks = {
    'jobs'  : [],
    'loops': [],
    'time'  : [],
}
pthread_atomic = {
    'jobs'  : [],
    'loops': [],
    'time'  : [],
}
pthread_nosync = {
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
data_array = data.split("[EXERCISE 3]")
for str in data_array:
    str = str.split("\n")
    if str != ['']:
        match str[1]:
            case 'type = locks':
                pthread_locks['jobs'].append(int(str[2].split(' = ')[1]))
                pthread_locks['loops'].append(int(str[3].split(' = ')[1]))
                pthread_locks['time'].append(float(str[4].split(' = ')[1]))
            case 'type = atomics':
                pthread_atomic['jobs'].append(int(str[2].split(' = ')[1]))
                pthread_atomic['loops'].append(int(str[3].split(' = ')[1]))
                pthread_atomic['time'].append(float(str[4].split(' = ')[1]))
            case 'type = no synchronization':
                pthread_nosync['jobs'].append(int(str[2].split(' = ')[1]))
                pthread_nosync['loops'].append(int(str[3].split(' = ')[1]))
                pthread_nosync['time'].append(float(str[4].split(' = ')[1]))


lock_dataframe   = pd.DataFrame(pthread_locks)
lock_dataframe   = lock_dataframe.sort_values(by = 'loops', ascending = True)
# we separate the dataframes by jobs, index is jobs-1
lock_dataframes   = lock_dataframe.groupby('loops')
lock_dataframes   = [lock_dataframe.loc[lock_dataframe['loops'] == i] for i in lock_dataframe['loops'].unique()]
lock_dataframes = [df.reset_index(drop = True) for df in lock_dataframes]
lock_dataframes = [df.drop(columns = ['loops']) for df in lock_dataframes]
lock_dataframes = [df.sort_values(by = 'jobs', ascending = True) for df in lock_dataframes]

# PTHREADS LOCKS
atomic_dataframe = pd.DataFrame(pthread_atomic)
atomic_dataframe = atomic_dataframe.sort_values(by = 'loops', ascending = True)
# we separate the dataframes by jobs, index is jobs-1
atomic_dataframes = atomic_dataframe.groupby('loops')
atomic_dataframes = [atomic_dataframe.loc[atomic_dataframe['loops'] == i] for i in atomic_dataframe['loops'].unique()]
atomic_dataframes = [df.reset_index(drop = True) for df in atomic_dataframes]
atomic_dataframes = [df.drop(columns = ['loops']) for df in atomic_dataframes]
atomic_dataframes = [df.sort_values(by = 'jobs', ascending = True) for df in atomic_dataframes]

# PTHREADS NO SYNC
nosync_dataframe = pd.DataFrame(pthread_nosync)
nosync_dataframe = nosync_dataframe.sort_values(by = 'loops', ascending = True)
# we separate the dataframes by jobs, index is jobs-1
nosync_dataframes = nosync_dataframe.groupby('loops')
nosync_dataframes = [nosync_dataframe.loc[nosync_dataframe['loops'] == i] for i in nosync_dataframe['loops'].unique()]
nosync_dataframes = [df.reset_index(drop = True) for df in nosync_dataframes]
nosync_dataframes = [df.drop(columns = ['loops']) for df in nosync_dataframes]
nosync_dataframes = [df.sort_values(by = 'jobs', ascending = True) for df in nosync_dataframes]

# PLOTTING
# Plot the locks data (time vs loops) on its own plot
plt.figure(figsize = (10, 5))
plt.title("Locks")
plt.xlabel("Number of jobs")
plt.ylabel("Execution time (msec)")
for i in range(len(lock_dataframes)):
    color = plt.cm.viridis(i / len(lock_dataframes))
    plt.plot(lock_dataframes[i]['jobs'], lock_dataframes[i]['time'], marker = 'o', color = color)
#add legend
    plt.legend([f"loops = {10000*10**(i + 1)}" for i in range(len(lock_dataframes))], loc = 'upper left')
plt.xticks(lock_dataframes[0]['jobs'])
plt.grid()
plt.savefig("data/artifacts/plot3_locks.png")
plt.close()
# Plot the atomic (time vs loops) on its own plot
plt.figure(figsize = (10, 5))
plt.title("Atomics")
plt.xlabel("Number of jobs")
plt.ylabel("Execution time (msec)")
for i in range(len(atomic_dataframes)):
    color = plt.cm.viridis(i / len(atomic_dataframes))
    plt.plot(atomic_dataframes[i]['jobs'], atomic_dataframes[i]['time'], marker = 'o', color = color)
#add legend
    plt.legend([f"loops = {10000*10**(i + 1)}" for i in range(len(atomic_dataframes))], loc = 'upper left')
plt.xticks(atomic_dataframes[0]['jobs'])
plt.grid()
plt.savefig("data/artifacts/plot3_atomic.png")
plt.close()
# Plot the no sync (time vs loops) on its own plot
plt.figure(figsize = (10, 5))
plt.title("No Synchronization")
plt.xlabel("Number of jobs")
plt.ylabel("Execution time (msec)")
for i in range(len(nosync_dataframes)):
    color = plt.cm.viridis(i / len(nosync_dataframes))
    plt.plot(nosync_dataframes[i]['jobs'], nosync_dataframes[i]['time'], marker = 'o', color = color)
#add legend
    plt.legend([f"loops = {10000*10**(i + 1)}" for i in range(len(nosync_dataframes))], loc = 'upper left')
plt.xticks(nosync_dataframes[0]['jobs'])
plt.grid()
plt.savefig("data/artifacts/plot3_nosync.png")
plt.close()

    
