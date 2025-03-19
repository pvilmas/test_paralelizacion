from matplotlib import pyplot as plt
import os
import pandas
import csv

current_dir = os.getcwd()
files = [f for f in os.listdir(current_dir) if os.path.isfile(f) and f.startswith("results") and f.endswith(".csv")]

csv_data = [(file.split("_")[1], pandas.read_csv(file, usecols=["period", "time"])) for file in files]

speedups = {}

secuencial = [data for data in csv_data if data[0] == "1"][0]

frequencies = [1.0/p for p in secuencial[1]["period"]]

parallelized = [data for data in csv_data if data[0] != "1"]

for partitions, data in parallelized:
    data["speedup"] = data["time"] / secuencial[1]["time"]
    data["period"] = frequencies
    data.to_csv(f'speedup_{partitions}.csv', index=False)
    plt.plot(data["period"], data["speedup"], label=f'{partitions} particiones')
    plt.scatter(data["period"], data["speedup"])

handles, labels = plt.gca().get_legend_handles_labels()
order = [4, 3, 1, 2, 0]
plt.legend([handles[idx] for idx in order], [labels[idx] for idx in order])

plt.title("Comparación del speedup para distintos conjuntos de particiones")
plt.xlabel("Frecuencia de generación de vehículos (1/s)")
plt.ylabel("Speedup (t_original/t_actual)")
plt.yscale('log')
plt.grid()

plt.show()

for partitions, data in parallelized:
    data["efficiency"] = data["speedup"] / int(partitions)
    data.to_csv(f'efficiency_{partitions}.csv', index=False)
    plt.plot(data["period"], data["efficiency"], label=f'{partitions} particiones')
    plt.scatter(data["period"], data["efficiency"])

handles, labels = plt.gca().get_legend_handles_labels()
order = [4, 3, 1, 2, 0]
plt.legend([handles[idx] for idx in order], [labels[idx] for idx in order])

plt.title("Comparación de la eficiencia de paralelización")
plt.xlabel("Frecuencia de generación de vehículos (1/s)")
plt.ylabel("Efficiency (speedup/partitions)")
plt.grid()

plt.show()
