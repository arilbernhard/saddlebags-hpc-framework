import matplotlib
import matplotlib.pyplot as plt
import numpy as np

font = {'family' : 'serif',
        'size'   : 18}

matplotlib.rc('font', **font)

upc = [26,106,113]
spark = [297, 1107, 2726]

x_axis_labels = [   "6000 articles\n~100MB",
                    "18000 articles\n~300MB",
                    "54000 articles\n~900MB"
]

plt.scatter(range(0,len(upc)), upc, label='UPC++', linewidths=3)
plt.scatter(range(0,len(spark)), spark, label='Spark', linewidths=3)

plt.plot(range(0,len(upc)), upc)
plt.plot(range(0,len(spark)), spark)

plt.legend()
plt.grid()

index = np.arange(len(x_axis_labels))
plt.xticks(index, x_axis_labels)

plt.title("Time (s) of TF-IDF on Wikipedia dataset\n16 nodes, 8GB memory @ Abel cluster")



plt.show()