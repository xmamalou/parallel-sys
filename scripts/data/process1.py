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
                serial_ex1['throws'].append(int(str[2].split(' = ')[1]))
                serial_ex1['π'].append(float(str[3].split(' = ')[1]))
                serial_ex1['time'].append(float(str[4].split(' = ')[1]))
            case 'type = pthreads':
                pthread_ex1['jobs'].append(int(str[2].split(' = ')[1]))
                pthread_ex1['throws'].append(int(str[3].split(' = ')[1]))
                pthread_ex1['π'].append(float(str[4].split(' = ')[1]))
                pthread_ex1['time'].append(float(str[5].split(' = ')[1]))
            case 'type = openmp':
                omp_ex1['jobs'].append(int(str[2].split(' = ')[1]))
                omp_ex1['throws'].append(int(str[3].split(' = ')[1]))
                omp_ex1['π'].append(float(str[4].split(' = ')[1]))
                omp_ex1['time'].append(float(str[5].split(' = ')[1]))

serial_dataframe   = pd.DataFrame(serial_ex1)
#
pthreads_dataframe = pd.DataFrame(pthread_ex1)
pthreads_dataframe = pthreads_dataframe.sort_values(by = 'jobs', ascending = True)
#
omp_dataframe      = pd.DataFrame(omp_ex1)
omp_dataframe      = omp_dataframe.sort_values(by = 'jobs', ascending = True)
#
print(serial_dataframe, '\n', pthreads_dataframe, '\n', omp_dataframe)