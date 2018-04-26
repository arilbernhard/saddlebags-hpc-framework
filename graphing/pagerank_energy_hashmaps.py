import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
from collections import namedtuple


n_groups = 6

#pure = [1.22195, 3.05374, 2.31333, 3.08149, 2.17764]

pure = [1.60725, 3.99753, 1.00217, 1.81586, 1.45144, 0.497917, 0.402918, 0.469166, 1.2214, 1.70857, 1.17485, 0.835944]

std = [3.22349, 1.41891, 0.724099, 3.46205, 3.43452]

robin5 = [2.86788, 1.31927, 1.16583, 1.69493, 1.2359]

robin7 = [2.7485, 2.05865, 1.81954, 2.75178, 1.35297]

open5 = [3.15963, 1.16494, 0.783021, 2.02455, 1.27226]

#open7 = [1.28744, 2.35759, 1.36043, 1.99177, 0.80094]
open7 = [0.437019, 0.678165, 0.887747, 1.61324, 1.02195, 0.341489, 0.472826, 0.731868, 0.860739, 1.23932]

pure_time = 101.7
pure_time_std = 0.5

std_time = 106.800
std_time_std = 0.941

robin5_time = 100.813
robin5_time_std = 0.483

robin7_time = 101.841
robin7_time_std = 2.801

open5_time = 101.982
open5_time_std = 1.285

open7_time = 100.755
open7_time_std = 0.610


means = [np.average(x) for x in [pure, std, robin5, robin7, open5, open7]]
std = [np.std(x) for x in [pure, std, robin5, robin7, open5, open7]]

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
ax.set_title('Power consumption and Execution time\n5 iterations of Pagerank, Kroenicker graph with 8.3 million nodes, 46 million edges\n24 GASNet partitions at 24 cores, Intel Xeon CPU E5-2650L v3 @ 1.80GHz')
ax.set_xticks(index + bar_width / 2)

index2 = np.arange(n_groups) + bar_width

ax2 = ax.twinx()
ax2.set_ylabel('Time (seconds)', color='r')
ax2.tick_params('y', colors='r')
ax2.set_ylim([91,110])

rects2 = ax2.bar(index2, [pure_time, std_time,robin5_time,robin7_time,open5_time,open7_time], bar_width,
                alpha=opacity, color='red',
                yerr=[pure_time_std, std_time_std,robin5_time_std,robin7_time_std, open5_time_std, open7_time_std], error_kw=error_config)


ax.set_xticklabels(('Pure UPC++','std::unordered_map', 'Robin Hood lf=0.5', 'Robin Hood lf=0.7', 'Open Addressing lf=0.5', 'Open Addressing lf=0.7'))
ax.legend()

fig.tight_layout()
plt.show()