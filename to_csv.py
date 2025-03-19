import os
import numpy as np
from matplotlib import pyplot as plt
import csv

current_dir = os.getcwd()

directories = [dir for dir in os.listdir(current_dir) if os.path.isdir(dir)]
directories.remove(".vscode")
print(directories)

results = {}
cpu_uses = {}
speedups = {}

# def get_cpu_usage(f):
#     lines = f.readlines()
#     important_line = lines[len(lines)-2]
#     important_info = important_line.split(" ")
#     print(important_info)
    
#     cpu_use = float(important_info[3].split("%")[0])
#     print(f'CPU usage: {cpu_use}')

#     return cpu_use

for directory in directories:
    os.chdir(directory)
    results_per_partition = []
    # results_cpu_per_partition = []
    results_per_period = {}
    sub_directories = [dir for dir in os.listdir() if os.path.isdir(dir)]
    for sub_directory in sub_directories:
        with open(f'{sub_directory}/output.txt') as file:
            res = file.read()
        result = res.split(':')
        period = result[0].split(' ')
        period = float(period[len(period)-1])
        time = float(result[1][1:])
        results_per_period = { "period": period,
                              "time": time }
        results_per_partition.append(results_per_period)

        # with open(f'{sub_directory}/CPU_usage_{period:.6f}.log', 'r') as f:
        #     print(f'current file: {f} for directory {directory}')
        #     cpu_use = get_cpu_usage(f)
        #     cpu_per_period = { period : cpu_use }
        #     results_cpu_per_partition.append(cpu_per_period)
    
    # cpu_uses[int(directory)] = results_cpu_per_partition
    results[int(directory)] = sorted(results_per_partition, key=lambda x : x["period"])
    os.chdir('..')        

for partition, result in results.items():
    print(result)
    with open(f'results_{partition}_partitions.csv', "w", newline="") as file:
        fieldnames = ["period", "time"]
        w = csv.DictWriter(file, fieldnames)
        w.writeheader()
        w.writerows(result)
    

# legends = [partition for partition in results.keys()]

# for partition, cpu_use in cpu_uses.items():
#     result = sorted(cpu_use, key=lambda x: list(x.keys())[0])
#     sim_cpu_freqs = [1/list(item.keys())[0] for item in cpu_use]
#     sim_cpu_use = [list(item.values())[0] for item in cpu_use]
#     plt.scatter(sim_cpu_freqs, sim_cpu_use, label=f'{partition}')

# handles, labels = plt.gca().get_legend_handles_labels()
# order = [2, 1, 4, 0, 3]
# plt.legend([handles[idx] for idx in order], [labels[idx] for idx in order])

# plt.xlabel("Generation frequency (in seconds)")
# plt.ylabel("CPU usage (%)")
# plt.title(f'Simulation CPU usage per vehicle generation frequency for different partitions')
# plt.grid()
# plt.show()

# for partition, res in results.items():
#     result = sorted(res, key=lambda x: list(x.keys())[0])
#     sim_times_freqs = [1/list(item.keys())[0] for item in result]
#     sim_times = [list(item.values())[0] for item in result]
#     print(sim_times_freqs)
#     print (sim_times)
#     plt.scatter(sim_times_freqs, sim_times, label=f'{partition}')
    


# plt.yscale("log")
# plt.ylim(1)
# plt.xlabel("Generation frequency (in seconds)")
# plt.ylabel("Execution time (in seconds)")
# plt.title(f'Simulation time per vehicle generation frequency for {partition} partitions')
# plt.grid()
# plt.show()


