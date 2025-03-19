#include <iostream>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <bits/stdc++.h>
#include <ctime>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;
using namespace std::chrono;

#ifdef __linux__
#include <sched.h>
#endif
/*
    Acá el test de carga tiene que hacer lo siguiente:
        - Tiene que obtener todos los archivos de configuración (están nombrados como sim_part_{partition}_{period}.sumocfg)
        - luego, con el número de particiones, el período, la imagen de singularity y el arreglo de archivos de configuración,
        se llama al dispatcher (esto se tiene que hacer un número grande de veces pero primero probar con un número pequeño)
        - medir el tiempo que toma cada ejecución
*/

double run_simulation(int num_partitions, double period, string singularity_image, vector<string> configuration_files){
    string prompt = "./dispatcher " + to_string(num_partitions) + " " + to_string(period) + " " + singularity_image;
    for(string conf_file : configuration_files){
        prompt.append(" ");
        prompt.append(conf_file);
    }
    vector<double> times;
    int iterations = 50;
    for(int i = 0; i < iterations; i++){
        int time;
        time_t start, end;
        ios_base::sync_with_stdio(false);
        const char* run_sim_char = prompt.c_str();
        string current_simulation = "Starting simulation " + to_string(i) + "/" + to_string(iterations) + " for period " + to_string(period); 
        cout << current_simulation << endl;
        string notification = "curl -d '" + current_simulation + "' ntfy.sh/pvillar_traffic_simulations_alerts";
        system(notification.c_str());
        std::time(&start);
        system(run_sim_char);
        std::time(&end);
        double time_taken = double(end-start);
        times.push_back(time_taken);
    }
    double mean_time = accumulate(times.begin(), times.end(), 0.0)/times.size();
    return mean_time;
}

int main(int argc, char const *argv[])
{
    if(argc < 2){
        cerr << "Usage " << argv[0] << "<num_partitions> <period> <singularity_image>";
        return 1;
    }

    #ifdef __linux__
        cpu_set_t mask;
        int status;

        CPU_ZERO(&mask);
        CPU_SET(0,&mask);
        status = sched_setaffinity(0,sizeof(mask),&mask);
        if (status != 0) {
            perror("sched_setaffinity");
        }
    #endif

    int num_partition = stoi(argv[1]);
    double period = stod(argv[2]);
    string singularity_image = argv[3];

    vector<string> config_files;
    for(int i = 0; i < num_partition; i++){
        string config_filename = "sim_part_" + to_string(i) + "_" + to_string(period) + ".sumocfg";
        config_files.push_back(config_filename);
    }

    double time = run_simulation(num_partition, period, singularity_image, config_files);

    ofstream out_file("output.txt");
    out_file << "Time for execution with period " << to_string(period) << ": " << to_string(time);
    out_file.close();

    return 0;
}
