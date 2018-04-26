import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
from collections import namedtuple


n_groups = 3

#pure = [1.22195, 3.05374, 2.31333, 3.08149, 2.17764]

pure = [9.62884, 11.9738,10.4448]

std = [15.1036, 5.14324, 6.81276]

robin5 = [0]

robin7 = [0]

open5 = [0]

#open7 = [1.28744, 2.35759, 1.36043, 1.99177, 0.80094]
open7 = [7.31574, 5.29366, 7.69692]

pure_time = np.average([376.626,379.812])
pure_time_std = np.std([376.626,379.812])

std_time = 436.994
std_time_std = 0

robin5_time = 0
robin5_time_std = 0

robin7_time = 0
robin7_time_std = 0

open5_time = 0
open5_time_std = 0

open7_time = np.average([446.159, 448])
open7_time_std = np.std([446.159, 448])


means = [np.average(x) for x in [pure, std, open7]]
std = [np.std(x) for x in [pure, std, open7]]

fig, ax = plt.subplots()

index = np.arange(n_groups)
bar_width = 0.35

opacity = 0.7
error_config = {'ecolor': '0'}

plt.xticks(rotation=20)

rects1 = ax.bar(index, means, bar_width,
                alpha=opacity, color='blue',
                yerr=std, error_kw=error_config)

ax.set_xlabel('Hash table configuration')
ax.set_ylabel('Power consumption (e-308 Joule)')
ax.set_title('Power consumption and Execution time\n5 iterations of Pagerank, Kroenicker graph with 67 million nodes, 460 million edges\n24 GASNet partitions at 24 cores, Intel Xeon CPU E5-2650L v3 @ 1.80GHz')
ax.set_xticks(index + bar_width / 2)

index2 = np.arange(n_groups) + bar_width

ax2 = ax.twinx()
ax2.set_ylabel('Time (seconds)', color='r')
ax2.tick_params('y', colors='r')
#ax2.set_ylim([91,110])

rects2 = ax2.bar(index2, [pure_time, std_time,open7_time], bar_width,
                alpha=opacity, color='red',
                yerr=[pure_time_std, std_time_std, open7_time_std], error_kw=error_config)


ax.set_xticklabels(('Pure UPC++','std::unordered_map', 'Open Addressing lf=0.7'))
ax.legend()

fig.tight_layout()
plt.show()