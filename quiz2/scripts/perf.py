import os
import matplotlib.pyplot as plt
import numpy as np

os.chdir("/home/rin/Github/system-software/NCKU-System-Software-Quiz2/")

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

old_time_collect = []
new_time_collect = []


for i in range(100):
    os.system("./fastdiv > ./plot/out.txt")
    out = np.loadtxt("./plot/out.txt", dtype = 'int',delimiter=',')
  
    old_time_collect.append(out[:,1])
    new_time_collect.append(out[:,2])

x = out[:,0]
old_time_collect = stat(old_time_collect)
new_time_collect = stat(new_time_collect)

fig , ax = plt.subplots()
plt.subplot(1,2, 1)
plt.xlabel('divisor =')
plt.ylabel('time(ns)')
plt.plot(x, old_time_collect, label = 'fastdiv')
plt.plot(x, new_time_collect, label = 'div')
plt.legend(loc = 'upper left')

plt.subplot(1, 2, 2)
plt.xlabel('divisor =')
plt.ylabel('time(ns)')
plt.plot(x[2:], old_time_collect[2:], label = 'fastdiv')
plt.plot(x[2:], new_time_collect[2:], label = 'div')
plt.legend(loc = 'upper left')
plt.show()
