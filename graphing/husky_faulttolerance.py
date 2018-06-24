import matplotlib
import matplotlib.pyplot as plt
import numpy as np
plt.figure(figsize=(13,9))

font = {'family' : 'serif',
        'size'   : 18}

matplotlib.rc('font', **font)

'''husky = [       [1.243], 
                [1.256],
                [1.128], 
                [1.382], 
                [1.383], 
                [1.764], 
                [1.995],
                [2.897],
                [5.109]]
'''

husky = [
                [1.995],
                [2.897],
                [5.109]]

old_upc = [     [0.106], 
                [0.107], 
                [0.181], 
                [0.201], 
                [0.5], 
                [2.270], 
                [11.951]]

new_upc = [     [0.109, 0.158, 0.155],
                [0.156, 0.107, 0.166],
                [0.178, 0.192, 0.129],
                [0.167, 0.162, 0.197],
                [0.203, 0.184, 0.216],
                [0.225, 0.245, 0.261],
                [0.358, 0.349, 0.354],
                [0.488, 0.543, 0.515],
                [0.936, 0.940, 1.009]]

pure_upc = [    [0.107, 0.107, 0.153],
                [0.107, 0.107, 0.163],
                [0.113, 0.230, 0.111],
                [0.164, 0.120, 0.176],
                [0.136, 0.136, 0.187],
                [0.173, 0.170, 0.217],
                [0.231, 0.231, 0.248],
                [0.395, 0.390, 0.434],
                [0.798, 0.791, 0.824]]

robin_upc = [
                [0.284, 0.318, 0.280],
                [0.460, 0.496, 0.467],
                [0.818, 0.814, 0.815]]


fault_one = [
                [0.336, 0.342, 0.393],
                [0.560, 0.552, 0.598],
                [1.079, 1.060, 1.097]]

fault_two = [
                [0.390, 0.371, 0.370],
                [0.684, 0.641, 0.651],
                [1.289, 1.246, 1.250]]

fault_three = [
                [0.418, 0.457, 0.446],
                [0.697, 0.750, 0.684],
                [1.463, 1.406, 1.413]]


'''
x_axis_labels = [   "4 iters\n16 nodes\n21 edges",
                    "6 iters\n64 nodes\n99 edges",
                    "10 iters\n1k nodes\n2k edges",
                    "12 iters\n4k nodes\n10k edges",
                    "13 iters\n8k nodes\n21k edges",
                    "14 iters\n16k nodes\n46k edges",
                    "15 iters\n32k nodes\n99k edges",
                    "16 iters\n65k nodes\n214k edges",
                    "17 iters\n131k nodes\n461k edges"]
'''

x_axis_labels = [   
                    "15 iters\n32k nodes\n99k edges",
                    "16 iters\n65k nodes\n214k edges",
                    "17 iters\n131k nodes\n461k edges"]


plt.grid()


#plt.scatter(range(0,len(old_upc)), [np.average(x) for x in old_upc], label='Old UPC++ Framework', linewidths=3)
#plt.scatter(range(0,len(new_upc)), [np.average(x) for x in new_upc], label='Fixed UPC++ Framework', linewidths=3)
#plt.scatter(range(0,len(pure_upc)), [np.average(x) for x in pure_upc], label='Pure UPC++', linewidths=3)
plt.scatter(range(0,len(husky)), [np.average(x) for x in husky], label='Husky', linewidths=3, color='RED')
plt.scatter(range(0,len(robin_upc)), [np.average(x) for x in robin_upc], label='Saddlebags (no replication)', linewidths=3, color='BLACK')
plt.scatter(range(0,len(fault_one)), [np.average(x) for x in fault_one], label='Saddlebags (replication degree 1)', linewidths=3, color='BLUE')
plt.scatter(range(0,len(fault_two)), [np.average(x) for x in fault_two], label='Saddlebags (replication degree 2)', linewidths=3, color='GREEN')
plt.scatter(range(0,len(fault_three)), [np.average(x) for x in fault_three], label='Saddlebags (replication degree 3)', linewidths=3, color='ORANGE')

#plt.plot(range(0,len(old_upc)), [np.average(x) for x in old_upc], linestyle=':')
#plt.plot(range(0,len(new_upc)), [np.average(x) for x in new_upc])
#plt.plot(range(0,len(pure_upc)), [np.average(x) for x in pure_upc])
#plt.plot(range(0,len(husky)), [np.average(x) for x in husky])

#plt.errorbar(range(0,len(old_upc)), [np.average(x) for x in old_upc], [np.std(x) for x in old_upc])
#plt.errorbar(range(0,len(new_upc)), [np.average(x) for x in new_upc], [np.std(x) for x in new_upc])
#plt.errorbar(range(0,len(pure_upc)), [np.average(x) for x in pure_upc], [np.std(x) for x in pure_upc])
plt.errorbar(range(0,len(husky)), [np.average(x) for x in husky], [np.std(x) for x in husky], color='RED')
plt.errorbar(range(0,len(robin_upc)), [np.average(x) for x in robin_upc], [np.std(x) for x in robin_upc], color='BLACK')
plt.errorbar(range(0,len(fault_one)), [np.average(x) for x in fault_one], [np.std(x) for x in fault_one], color='BLUE')
plt.errorbar(range(0,len(fault_two)), [np.average(x) for x in fault_two], [np.std(x) for x in fault_two], color='GREEN')
plt.errorbar(range(0,len(fault_three)), [np.average(x) for x in fault_three], [np.std(x) for x in fault_three], color='ORANGE')



plt.legend()


index = np.arange(len(x_axis_labels))
plt.xticks(index, x_axis_labels)

plt.title("Time (s) of 3 iterations of PageRank on Kronecker graph (BigDataBench 3.2)\nSingle node, 8GB memory, 4 cores")


#plt.savefig('replication.png', format='png', dpi=500)

plt.show()