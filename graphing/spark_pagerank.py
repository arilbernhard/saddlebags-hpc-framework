import matplotlib
import matplotlib.pyplot as plt
import numpy as np

font = {'family' : 'serif',
        'size'   : 18}

matplotlib.rc('font', **font)
'''
upc = [2.8, 4.84, 6.4, 16.9]
spark = [19.2, 23.0, 36.1, 55.6]
graphx = [14.2, 17.9, 27.9, 48.3]

x_axis_labels = [       "14\n16k n, 46k e",
                        "15\n32k n, 99k e",
                        "16\n65k n, 214k e",
                        "17\n131k n, 461k e"]
     '''


upc = [5.62, 18.9]
spark = [76, 774]
graphx = [51, 425]

x_axis_labels = [   "17 iters\n131k nodes\n461k edges",
                    "20 iters\n1.04m nodes\n4.61m edges"]

width = 0.25

ind = np.arange(2)

plt.bar(ind, upc, width,label='Saddlebags', color='BLACK')
plt.bar(ind+width, spark, width,label='Spark RDD MapReduce', color='RED')
plt.bar(ind+(2*width), graphx, width,label='Spark Graphx', color='BLUE')


plt.legend()
#plt.grid()

index = np.arange(len(x_axis_labels))
plt.xticks(index+width, x_axis_labels)

plt.title("Time (s) of PageRank on Kronecker graph (BigDataBench 3.2) \n16 nodes, 8GB memory")

plt.grid(axis='y')


plt.show()