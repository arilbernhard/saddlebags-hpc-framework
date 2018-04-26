import matplotlib
import matplotlib.pyplot as plt
import numpy as np

font = {'family' : 'serif',
        'size'   : 18}

matplotlib.rc('font', **font)

upc = [2.8, 4.84, 6.4, 16.9]
spark = [19.2, 23.0, 36.1, 55.6]
graphx = [14.2, 17.9, 27.9, 48.3]

x_axis_labels = [       "14\n16k n, 46k e",
                        "15\n32k n, 99k e",
                        "16\n65k n, 214k e",
                        "17\n131k n, 461k e"]
                
plt.scatter(range(0,len(upc)), upc, label='UPC++', linewidths=3)
plt.scatter(range(0,len(spark)), spark, label='Spark RDD MapReduce', linewidths=3)
plt.scatter(range(0,len(graphx)), graphx, label='Spark Graphx', linewidths=3)

plt.plot(range(0,len(upc)), upc)
plt.plot(range(0,len(spark)), spark)
plt.plot(range(0,len(graphx)), graphx)


plt.legend()
plt.grid()

index = np.arange(len(x_axis_labels))
plt.xticks(index, x_axis_labels)

plt.title("Time (s) of PageRank on Kronecker graph (BigDataBench 3.2) \n16 nodes, 8GB memory @ Abel cluster")



plt.show()