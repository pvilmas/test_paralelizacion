#include <iostream>
#include <fstream>
#include <unordered_map>
#include <omp.h>
#include "tinyxml2.h"
#include "json.hpp"

using namespace std;
using namespace tinyxml2;
using json = nlohmann::json;

// Shared vehicle data across partitions
unordered_map<string, json> outgoing_vehicle_data;
#pragma omp private(outgoing_vehicle_data)

void start_partition_simulation(int partition_id, double period, const string &config_file, const string &singularity_image) {
    // Command to run the SUMO simulation in a Singularity container
    string command = "singularity exec " + singularity_image + " sumo -c " + config_file;
    system(command.c_str());
}

void handle_outgoing_vehicles(int partition_id, const string &outgoing_file, const string &incoming_file) {
    json outgoing_data;
    json incoming_data;

    #pragma omp critical
    {
        if (outgoing_vehicle_data.count(to_string(partition_id))) {
            outgoing_data = outgoing_vehicle_data[to_string(partition_id)];

            // Write outgoing data to a file
            ofstream outgoing_stream(outgoing_file);
            if (outgoing_stream.is_open()) {
                outgoing_stream << outgoing_data.dump();
                outgoing_stream.close();
            }

            // Simulate receiving incoming data
            ifstream incoming_stream(incoming_file);
            if (incoming_stream.is_open()) {
                incoming_stream >> incoming_data;
                incoming_stream.close();

                // Process incoming vehicles
                for (const auto &vehicle : incoming_data["vehicles"]) {
                    cout << "Partition " << partition_id << " received vehicle: " << vehicle["id"] << endl;
                }
            }

            // Clear processed data
            outgoing_vehicle_data.erase(to_string(partition_id));
        }
    }
}

void run_simulation(int num_partitions, double period, const vector<string> &config_files, const vector<string> &outgoing_files, const vector<string> &incoming_files, const string &singularity_image) {
    // Start parallel regions
    #pragma omp parallel
    {
        #pragma omp for
        for (int i = 0; i < num_partitions; ++i) {
            // Start simulation for each partition
            start_partition_simulation(i, period, config_files[i], singularity_image);
        }

        #pragma omp for
        for (int i = 0; i < num_partitions; ++i) {
            // Handle outgoing and incoming vehicle data for each partition
            handle_outgoing_vehicles(i, outgoing_files[i], incoming_files[i]);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        cerr << "Usage: " << argv[0] << " <num_partitions> <period> <singularity_image> <config_file_1> ... <config_file_n>" << endl;
        return 1;
    }

    int num_partitions = stoi(argv[1]);
    double period = stod(argv[2]);
    string singularity_image = argv[3];

    if (argc != 4 + num_partitions) {
        cerr << "Error: Provide one config file per partition." << argc << endl;
        return 1;
    }

    vector<string> config_files;
    vector<string> outgoing_files(num_partitions);
    vector<string> incoming_files(num_partitions);

    for (int i = 0; i < num_partitions; ++i) {
        config_files.push_back(argv[4 + i]);
        outgoing_files[i] = "partition_" + to_string(i) + "_outgoing.json";
        incoming_files[i] = "partition_" + to_string(i) + "_incoming.json";
    }

    // Set the number of threads for OpenMP
    omp_set_num_threads(num_partitions);

    // Start the distributed simulation
    run_simulation(num_partitions, period, config_files, outgoing_files, incoming_files, singularity_image);

    cout << "Simulation completed." << endl;
    return 0;
}
