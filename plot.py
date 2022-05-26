import matplotlib.pyplot as plt
import numpy as np

f = open("rec.txt", "r")
x=0
values=[]
_y=[]
num_of_packets=0
for line in f:
    x=x+float(line.strip())
    num_of_packets=num_of_packets+1
    if(x>0.01):
        values.append(num_of_packets)
        _y.append(x)
        num_of_packets=0
        x=0

X=np.array(values)
Y=np.array(_y)

plt.scatter(X, Y)
plt.show()