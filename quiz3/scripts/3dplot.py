from mpl_toolkits import mplot3d
import numpy as np
import matplotlib.pyplot as plt
 
out = np.loadtxt("./out.txt", dtype = 'int', delimiter=',')

# Creating figure
fig = plt.figure(figsize = (16, 9))
ax = plt.axes(projection ="3d")
 
# Creating plot
ax.set_zlim3d(0,2000)
ax.scatter3D(out[:,0], out[:,1], out[:,2], label="fast gcd",\
        color = "green",marker ='^')
ax.scatter3D(out[:,0], out[:,1], out[:,3], label="faster gcd",\
        color = "red", marker ='*')
plt.title("time of gcd(a,b)")
ax.set_xlabel('experiment', fontweight ='bold') 
ax.set_ylabel('ctz of a and b', fontweight ='bold') 
ax.set_zlabel('time(ns)', fontweight ='bold')
ax.legend()

# show plot
plt.show()
