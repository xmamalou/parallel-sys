import sys
import os
from enum import Enum

import matplotlib.pyplot as plt
import numpy as np

# TYPES
class ExerciseOneParams:
    jobs   = []
    throws = []
    pi     = []
    time   = []

# READ FILE
data_filepath = os.getcwd() + "/" + sys.argv[1]
data_file = open(data_filepath)

data = data_file.read()

data_file.close()

# PARSE CONTENT
serial_ex1  = ExerciseOneParams()
pthread_ex1 = ExerciseOneParams()
omp_ex1     = ExerciseOneParams()

data_array = data.split("[EXERCISE 1]")
for str in data_array:
    str = str.split("\n")
    if str != ['']:
        match str[1]:
            case 'type = serial':
                for field in str:
                    if field == '': continue
                    if field == 'type = serial': continue

                    field = field.split(' = ')
                    match field[0]:
                        case 'throws':
                            serial_ex1.throws.append(field[1])
                        case 'π':
                            serial_ex1.pi.append(field[1])
                        case 'time':
                            serial_ex1.time.append(field[1])
            case 'type = pthreads':
                for field in str:
                    if field == '': continue
                    if field == 'type = pthread': continue
                    
                    field = field.split(' = ')
                    match field[0]:
                        case 'throws':
                            pthread_ex1.throws.append(field[1])
                        case 'π':
                            pthread_ex1.pi.append(field[1])
                        case 'time':
                            pthread_ex1.time.append(field[1])
                        case 'jobs':
                            pthread_ex1.jobs.append(field[1])
            case 'type = omp':
                for field in str:
                    if field == '': continue
                    if field == 'type = omp': continue
                    
                    field = field.split(' = ')
                    match field[0]:
                        case 'throws':
                            omp_ex1.throws.append(field[1])
                        case 'π':
                            omp_ex1.pi.append(field[1])
                        case 'time':
                            omp_ex1.time.append(field[1])

x_ticks = [i for i in range(1000000, 50000000, 1000000)]
y_ticks = [i/10000 for i in range(31300, 31500, 100)]

figure = plt.figure()
ax = figure.add_subplot(2, 1, 1)
ax.tick_params(labelrotation=45)
ax.set_yticks(y_ticks, labels=y_ticks)
ax.set_xticks(x_ticks, labels=x_ticks)
ax.plot(serial_ex1.throws, serial_ex1.pi, 'o')
plt.show()