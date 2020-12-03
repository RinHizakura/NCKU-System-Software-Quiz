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

ins_time_collect = []
merge_time_collect = []
tiled_time_collect = []


for i in range(50):
    os.system("taskset -c 5 ./sort_list.out > ./plot/out.txt")
    out = np.loadtxt("./plot/out.txt", dtype = 'float',delimiter=',')
  
    ins_time_collect.append(out[:,1])
    merge_time_collect.append(out[:,2])
    tiled_time_collect.append(out[:,2])

x = out[:,0]
ins_time_collect = stat(ins_time_collect)
merge_time_collect = stat(merge_time_collect)
tiled_time_collect = stat(tiled_time_collect)


plt.plot()
plt.xlabel('node #')
plt.ylabel('time(ms)')

plt.plot(x, ins_time_collect, label = 'insertion sort')
plt.plot(x, merge_time_collect, label = 'merge sort')
plt.plot(x, tiled_time_collect, label = 'tiled sort')
plt.legend(loc = 'upper left')
plt.show()
