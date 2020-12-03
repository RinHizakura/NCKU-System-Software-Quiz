import os
import matplotlib.pyplot as plt
import numpy as np


def stat(data):
    data = np.array(data)
    ret = np.zeros(data.shape[1])

    for i in range(data.shape[1]):
        tmp = data[:,i]
        mean, std = tmp.mean(), tmp.std()
        # identify outliers: 2 std = 95%
        cut_off = std * 2
        lower, upper = mean - cut_off, mean + cut_off
        tmp = tmp[tmp > lower]
        tmp = tmp[tmp < upper]
        ret[i] = tmp.mean()

    return ret

utime_collect = []

for i in range(50):
    os.system("taskset -c 5 ./sort_list.out> ./plot/THR.txt")
    out = np.loadtxt("./plot/THR.txt", dtype = 'float',delimiter=',')
  
    utime_collect.append(out[:,1])

x = out[:,0]
utime_collect = stat(utime_collect)


plt.plot()
plt.xlabel('threshold')
plt.ylabel('time(ns)')

plt.plot(x, utime_collect, marker = '*', markersize = 3)
plt.axhline(y=utime_collect[0], color='r', linestyle='-')
plt.show()
