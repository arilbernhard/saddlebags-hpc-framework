import numpy as np
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator
from collections import namedtuple


n_groups = 5


std = [46000000.0 / 3.22349, 46000000.0 / 1.41891, 46000000.0 / 0.724099, 46000000.0 / 3.46205, 46000000.0 / 3.43452]

robin5 = [46000000.0 / 2.86788, 46000000.0 / 1.31927, 46000000.0 / 1.16583, 46000000.0 / 1.69493, 46000000.0 / 1.2359]

robin7 = [46000000.0 / 2.7485, 46000000.0 / 2.05865, 46000000.0 / 1.81954, 46000000.0 / 2.75178, 46000000.0 / 1.35297]

open5 = [46000000.0 / 3.15963, 46000000.0 / 1.16494, 46000000.0 / 0.783021, 46000000.0 / 2.02455, 46000000.0 / 1.27226]

open7 = [46000000.0 / 1.28744, 46000000.0 / 2.35759, 46000000.0 / 1.36043, 46000000.0 / 1.99177, 46000000.0 / 0.80094]


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


means = [np.average(x) for x in [std, robin5, robin7, open5, open7]]
std = [np.std(x) for x in [std, robin5, robin7, open5, open7]]


energy_efficiency = [std_time / means[0], robin5_time / means[1], robin7_time / means[2], open5_time / means[3], open7_time / means[4]]
ee_std = [std_time_std / std[0], robin5_time_std / std[1], robin7_time_std / std[2], open5_time_std / std[3], open7_time_std / std[4]]

fig, ax = plt.subplots()

index = np.arange(n_groups)
bar_width = 0.35

opacity = 0.7
error_config = {'ecolor': '0'}

plt.xticks(rotation=20)

rects1 = ax.bar(index, energy_efficiency, bar_width, alpha=opacity, color='blue', yerr=ee_std, error_kw=error_config)

ax.set_xlabel('Hash table configuration')
ax.set_ylabel('Power consumption (e-308 Joule)')
ax.set_title('Power Consumption and Execution time\nPageRank, 5 iterations, Kroenicker graph, 8.3 million nodes, 46 million edges\n24 cores')
ax.set_xticks(index + bar_width / 2)


ax.set_xticklabels(('std::unordered_map', 'Robin Hood lf=0.5', 'Robin Hood lf=0.7', 'Open Addressing lf=0.5', 'Open Addressing lf=0.7'))
ax.legend()

fig.tight_layout()
plt.show()