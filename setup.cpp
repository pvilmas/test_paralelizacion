#include <iostream>
#include <fstream>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <bits/stdc++.h>
#include <ctime>
#include <filesystem>

using namespace std;
using namespace std::chrono;
namespace fs = std::filesystem;

void gen_periods(vector<double>& vec, double ini, double fin, double inc){
    double e = 0.0000001f;
    for (double i = ini + e; i >= fin ; i -= inc) {
        cout << "Next generated period: " << i << "\n";
        vec.push_back(i);
    }
}

int main(int argc, char const *argv[]){
    if(argc < 3){
        cerr << "Usage: " << argv[0] << " <num_partitions> <original_netfile>" << endl;
        return 1;
    }

    int num_partitions = stoi(argv[1]);
    string original_netfile = argv[2];
    // Generate periods
    vector<double> periods;
    gen_periods(periods, 1.0, 0.05, 0.05);

    for(double period : periods){
        string dirname = "period_" + to_string(period);
        string make_directory = "mkdir " + dirname;
        system(make_directory.c_str());
        string gen_routes = "python3 $SUMO_HOME/tools/randomTrips.py -n " + original_netfile + " --seed 13 -p " + to_string(period) + " -o trips_" + to_string(period) + ".trips.xml";
        system(gen_routes.c_str());
        string duarouter = "duarouter -n " + original_netfile + " --route-files trips_" + to_string(period) + ".trips.xml -o routes_" + to_string(period) + ".rou.xml --ignore-errors";
        system(duarouter.c_str());

        for(int i = 0 ; i < num_partitions; i++){
            string base_filename = "partition_" + to_string(i);
            string partition_filename = base_filename + ".net.xml";
            string partition_route_filename = base_filename + "_" + to_string(period) +".rou.xml";
            string cut_routes = "python3 $SUMO_HOME/tools/route/cutRoutes.py " + partition_filename + " routes_" + to_string(period) + ".rou.xml --routes-output " + partition_route_filename + " --orig-net " + original_netfile;
            system(cut_routes.c_str());
            string gen_config = "python3 generateConfig.py " + to_string(period) + " " + to_string(i);
            system(gen_config.c_str());
            string mv_routes = "mv " + partition_route_filename + " " + dirname;
            system(mv_routes.c_str());
            string mv_config = "mv sim_part_" + to_string(i) + "_" + to_string(period) +".sumocfg " + dirname;
            system(mv_config.c_str()); 
            string copy_partitions = "cp " + partition_filename + " " + dirname;
            system(copy_partitions.c_str());
        }
    }


}