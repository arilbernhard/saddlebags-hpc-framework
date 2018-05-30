import matplotlib
import matplotlib.pyplot as plt
import numpy as np
plt.figure(figsize=(11,9))

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


upc = [[0.25,0.254,0.254], [0.449,0.441,0.460], [0.808, 0.804, 0.813]]
husky = [[1.995, 2.0, 1.91],[2.897, 2.95, 2.85],[5.109, 5.0, 5.22]]
upc_ordered = [[0.260,0.266,0.280],[0.520,0.448,0.445],[0.882, 0.822, 0.817]]

x_axis_labels = [   
                    "15 iters\n32k nodes\n99k edges",
                    "16 iters\n65k nodes\n214k edges",
                    "17 iters\n131k nodes\n461k edges"]

width = 0.25

ind = np.arange(3)

plt.bar(ind, [np.average(x) for x in upc] , width,label='Saddlebags', color='BLACK', yerr=[np.std(x) for x in upc])
plt.bar(ind+width, [np.average(x) for x in upc_ordered], width,label='Spark RDD MapReduce', color='RED', yerr=[np.std(x) for x in upc_ordered])
plt.bar(ind+(2*width), [np.average(x) for x in husky], width,label='Spark Graphx', color='BLUE', yerr=[np.std(x) for x in husky])


plt.legend()
#plt.grid()

index = np.arange(len(x_axis_labels))
plt.xticks(index+width, x_axis_labels)

plt.title("Time (s) of PageRank on Kronecker graph (BigDataBench 3.2) \n16 nodes, 8GB memory")

plt.grid(axis='y')

plt.savefig('messageordering.png', format='png', dpi=500)

#plt.show()