import os
import matplotlib.pyplot as plt
import numpy as np

os.chdir("/home/rin/Github/system-software/NCKU-System-Software-Quiz5/")

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
    os.system("./sqrt.out > ./plot/out.txt")
    out = np.loadtxt("./plot/out.txt", dtype = 'int',delimiter=',')
  
    old_time_collect.append(out[:,3])
    new_time_collect.append(out[:,4])

x = out[:,0]
old_time_collect = stat(old_time_collect)
new_time_collect = stat(new_time_collect)

plt.plot()
plt.xlabel('sqrt(n), n=')
plt.ylabel('time(ns)')
plt.plot(x, old_time_collect, label = 'ieee754 + binary search')
plt.plot(x, new_time_collect, label = 'newton method')
plt.legend(loc = 'upper left')
plt.show()
