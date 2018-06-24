import matplotlib
import matplotlib.pyplot as plt
import numpy as np
plt.figure(figsize=(12,6))

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


upc = [[73.15,107,103.7]]
upc_ordered = [[117,74.51,102.36]]
#spark = [[1107]]

x_axis_labels = [   
                    "18000 articles\n~300MB"]

width = 0.25

ind = np.arange(1)

plt.bar(ind, [np.average(x) for x in upc] , width,label='Saddlebags', color='BLACK', yerr=[np.std(x) for x in upc])
plt.bar(ind+width, [np.average(x) for x in upc_ordered], width,label='Saddlebags (ordered)', color='RED', yerr=[np.std(x) for x in upc_ordered])
#plt.bar(ind+(2*width), [np.average(x) for x in spark], width,label='Spark', color='BLUE', yerr=83)

print [np.average(x) for x in upc]
print [np.average(x) for x in upc_ordered]
plt.legend()
#plt.grid()

index = np.arange(len(x_axis_labels))
plt.xticks(index+width/2, x_axis_labels)

plt.title("Time (s) of TF-IDF on Wikipedia articles \n16 nodes, 8GB memory")

plt.grid(axis='y')
plt.ylim(ymax=170)

#plt.savefig('messageordering.png', format='png', dpi=500)

plt.show()