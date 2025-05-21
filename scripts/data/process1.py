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
serial_ex1  = {
    'throws': [],
    'π'     : [],
    'time'  : [],
}
pthread_ex1 = {
    'jobs'  : [],
    'throws': [],
    'π'     : [],
    'time'  : [],
}
omp_ex1     = {
    'jobs'  : [],
    'throws': [],
    'π'     : [],
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
data_array = data.split("[EXERCISE 1]")
for str in data_array:
    str = str.split("\n")
    if str != ['']:
        match str[1]:
            case 'type = serial':
                serial_ex1['throws'].append(int(str[3].split(' = ')[1]))
                serial_ex1['π'].append(float(str[4].split(' = ')[1]))
                serial_ex1['time'].append(float(str[5].split(' = ')[1]))
            case 'type = parallel with pthreads':
                pthread_ex1['jobs'].append(int(str[2].split(' = ')[1]))
                pthread_ex1['throws'].append(int(str[3].split(' = ')[1]))
                pthread_ex1['π'].append(float(str[4].split(' = ')[1]))
                pthread_ex1['time'].append(float(str[5].split(' = ')[1]))
            case 'type = parallel with OpenMP':
                omp_ex1['jobs'].append(int(str[2].split(' = ')[1]))
                omp_ex1['throws'].append(int(str[3].split(' = ')[1]))
                omp_ex1['π'].append(float(str[4].split(' = ')[1]))
                omp_ex1['time'].append(float(str[5].split(' = ')[1]))

serial_dataframe   = pd.DataFrame(serial_ex1)
# PTHREADS
pthreads_dataframe = pd.DataFrame(pthread_ex1)
pthreads_dataframe = pthreads_dataframe.sort_values(by = 'jobs', ascending = True)
# we separate the dataframes by jobs, index is jobs-1
pthreads_dataframes = pthreads_dataframe.groupby('jobs')
pthreads_dataframes = [pthreads_dataframe.loc[pthreads_dataframe['jobs'] == i] for i in pthreads_dataframe['jobs'].unique()]
pthreads_dataframes = [df.reset_index(drop = True) for df in pthreads_dataframes]
pthreads_dataframes = [df.drop(columns = ['jobs']) for df in pthreads_dataframes]
pthreads_dataframes = [df.sort_values(by = 'throws', ascending = True) for df in pthreads_dataframes]
# OPENMP
omp_dataframe     = pd.DataFrame(omp_ex1)
omp_dataframe     = omp_dataframe.sort_values(by = 'jobs', ascending = True)
# we separate the dataframes by jobs, index is jobs-1
omp_dataframes     = omp_dataframe.groupby('jobs')
omp_dataframes     = [omp_dataframe.loc[omp_dataframe['jobs'] == i] for i in omp_dataframe['jobs'].unique()]
omp_dataframes     = [df.reset_index(drop = True) for df in omp_dataframes]
omp_dataframes     = [df.drop(columns = ['jobs']) for df in omp_dataframes]
omp_dataframes     = [df.sort_values(by = 'throws', ascending = True) for df in omp_dataframes]

# PLOTTING
# we first create histograms for each method for each number of throws, to show how much time each method took
# graphs on the same plot have different colors
plt.figure(figsize = (20, 10))
plt.suptitle("Execution time for each method", fontsize = 20)
plt.subplot(1, 3, 1)
plt.title("Serial")
plt.xscale('log')
plt.xlabel("Number of throws")
plt.ylabel("Execution time (s)")
plt.plot(serial_dataframe['throws'], serial_dataframe['time'], marker = 'o', color = 'blue')
plt.xticks(serial_dataframe['throws'])
plt.grid()
plt.subplot(1, 3, 2)
plt.title("Pthreads")
plt.xscale('log')
plt.xlabel("Number of throws")
plt.ylabel("Execution time (s)")
for i in range(len(pthreads_dataframes)):
    # different colors for each job
    color = plt.cm.viridis(i / len(pthreads_dataframes))
    plt.plot(pthreads_dataframes[i]['throws'], pthreads_dataframes[i]['time'], marker = 'o', color = color)
#add legend
    plt.legend([f"jobs = {i + 1}" for i in range(len(pthreads_dataframes))], loc = 'upper left')
plt.xticks(pthreads_dataframes[i]['throws'])
plt.grid()
plt.subplot(1, 3, 3)
plt.title("OpenMP")
plt.xscale('log')
plt.xlabel("Number of throws")
plt.ylabel("Execution time (msec)")
for i in range(len(omp_dataframes)):
    color = plt.cm.viridis(i / len(omp_dataframes))
    plt.plot(omp_dataframes[i]['throws'], omp_dataframes[i]['time'], marker = 'o', color = color)
#add legend
    plt.legend([f"jobs = {i + 1}" for i in range(len(omp_dataframes))], loc = 'upper left')
plt.xticks(omp_dataframes[i]['throws'])
plt.grid()
plt.savefig("data/artifacts/plot1.png")
plt.close()

# Plot the serial data (time vs throws) on its own plot
plt.figure(figsize = (10, 5))
plt.title("Serial")
plt.xscale('log')
plt.xlabel("Number of throws")
plt.ylabel("Execution time (msec)")
plt.plot(serial_dataframe['throws'], serial_dataframe['time'], marker = 'o', color = 'blue')
plt.xticks(serial_dataframe['throws'])
plt.grid()
plt.savefig("data/artifacts/plot1_serial.png")
plt.close()
# Plot the pthreads data (time vs throws) on its own plot
plt.figure(figsize = (10, 5))
plt.title("Pthreads")
plt.xscale('log')
plt.xlabel("Number of throws")
plt.ylabel("Execution time (msec)")
for i in range(len(pthreads_dataframes)):
    color = plt.cm.viridis(i / len(pthreads_dataframes))
    plt.plot(pthreads_dataframes[i]['throws'], pthreads_dataframes[i]['time'], marker = 'o', color = color)
#add legend
    plt.legend([f"jobs = {i + 1}" for i in range(len(pthreads_dataframes))], loc = 'upper left')
plt.xticks(pthreads_dataframes[i]['throws'])
plt.grid()
plt.savefig("data/artifacts/plot1_pthreads.png")
plt.close()
# Plot the OpenMP data (time vs throws) on its own plot
plt.figure(figsize = (10, 5))
plt.title("OpenMP")
plt.xscale('log')
plt.xlabel("Number of throws")
plt.ylabel("Execution time (s)")
for i in range(len(omp_dataframes)):
    color = plt.cm.viridis(i / len(omp_dataframes))
    plt.plot(omp_dataframes[i]['throws'], omp_dataframes[i]['time'], marker = 'o', color = color)
#add legend
    plt.legend([f"jobs = {i + 1}" for i in range(len(omp_dataframes))], loc = 'upper left')
plt.xticks(omp_dataframes[i]['throws'])
plt.grid()
plt.savefig("data/artifacts/plot1_omp.png")
plt.close()

    
