import matplotlib
import matplotlib.pyplot as plt
import numpy as np

plt.figure(figsize=(13,9))


font = {'family' : 'serif',
        'size'   : 18}

matplotlib.rc('font', **font)

'''

direct = [
            [0.792,0.798,0.85],
            [6.37,6.33,6.66],
            [66.71,66.77,66.76]
        ]

bufferingworker = [
            [0.8, 0.81, 0.81],
            [6.80,6.82,6.83],
            [69.20,74.11,69.14]
        ]

gasnetbuffering = [
            [0.84,0.83,0.88],
            [8.41,8.42,8.51],
            [108.54, 109.6, 111.49]
        ]
    
combining = [
            [0.63,0.63,0.69],
            [6.58,6.50,6.45],
            [78.32, 78.77, 78.26]
        ]

'''



direct = [
            [7.49, 7.23, 7.16],
            [21.24, 21.23, 23.73],
            [177.1, 183.0, 213.03, 150.05]
        ]

bufferingworker = [
            [5.62, 5.62, 5.71],
            [18.90, 19.08, 19.38],
            [163.03, 189.84, 178.68]
        ]

gasnetbuffering = [
            [5.14, 5.37, 5.29],
            [16.16, 16.66, 16.57],
            [138.19, 139.62, 141.21]
        ]
    
combining = [
            [4.42, 13.98, 14.41, 4.1],
            [8.40,9.34,7.91],
            [58.17, 63.46, 76.03]
        ]



plt.grid()


x_axis_labels = [   "17 iters\n131k nodes\n461k edges",
                    "20 iters\n1.04m nodes\n4.61m edges",
                    "23 iters\n8.38m nodes\n46m edges"]

plt.scatter(range(0,len(bufferingworker)), [np.average(x) for x in bufferingworker], label='Buffering', linewidths=3, color='RED')
plt.scatter(range(0,len(combining)), [np.average(x) for x in combining], label='Combining', linewidths=3, color='GREEN')
plt.scatter(range(0,len(direct)), [np.average(x) for x in direct], label='Direct', linewidths=3, color='BLACK')
plt.scatter(range(0,len(gasnetbuffering)), [np.average(x) for x in gasnetbuffering], label='GasnetBuffering', linewidths=3, color='BLUE')

#plt.plot(range(0,len(old_upc)), [np.average(x) for x in old_upc], linestyle=':')
#plt.plot(range(0,len(new_upc)), [np.average(x) for x in new_upc])
#plt.plot(range(0,len(pure_upc)), [np.average(x) for x in pure_upc])
#plt.plot(range(0,len(husky)), [np.average(x) for x in husky])

plt.errorbar(range(0,len(bufferingworker)), [np.average(x) for x in bufferingworker], [np.std(x) for x in bufferingworker], color='RED')
plt.errorbar(range(0,len(combining)), [np.average(x) for x in combining], [np.std(x) for x in combining], color='GREEN')
plt.errorbar(range(0,len(direct)), [np.average(x) for x in direct], [np.std(x) for x in direct], color='BLACK')
plt.errorbar(range(0,len(gasnetbuffering)), [np.average(x) for x in gasnetbuffering], [np.std(x) for x in gasnetbuffering], color='BLUE')



plt.legend()

index = np.arange(len(x_axis_labels))
plt.xticks(index, x_axis_labels)

plt.title("Time (s) of 3 iterations of PageRank on Kronecker graph (BigDataBench 3.2)\n16 nodes with 8GB memory")



#plt.show()
plt.savefig('sending_modes_cluster.png', format='png', dpi=500)
