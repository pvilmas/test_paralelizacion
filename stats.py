from matplotlib import pyplot as plt
import csv
import os
import pandas

current_dir = os.getcwd()
files = [f for f in os.listdir(current_dir) if os.path.isfile(f) and f.startswith("results") and f.endswith(".csv")]

print(files)

csv_data = [(f'{file}', pandas.read_csv(file, usecols=["period", "time"])) for file in files]

print(csv_data)
frecuencies = [1.0/p for p in csv_data[0][1]["period"]]

for partitions, data in csv_data:
    label = partitions.split("_")[1]
    data["period"] = frecuencies
    plt.plot(data["period"], data["time"], label=f'{label} particiones')
    plt.scatter(data["period"], data["time"])

handles, labels = plt.gca().get_legend_handles_labels()
order = [5, 4, 3, 1, 2, 0]
plt.legend([handles[idx] for idx in order], [labels[idx] for idx in order])

plt.title("Comparación de los tiempos de simulación para distintas cantidades de particiones")
plt.xlabel("Frecuencia de generación de vehículos (1/s)")
plt.ylabel("Tiempo de simulación (s)")
plt.grid()

plt.show()
