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

void gen_periods(vector<double>& vec, double ini, double fin, double inc){
    double e = 0.0000001f;
    for (double i = ini + e; i >= fin ; i -= inc) {
        cout << "Next generated period: " << i << "\n";
        vec.push_back(i);
    }
}

int main(int argc, char const *argv[])
{
    if(argc < 3){
        cerr << "Usage " << argv[0] << " <num_partitions> <singularity_image>";
        return 1;
    }

    string num_partitions = argv[1];
    string singularity_image = argv[2];

    vector<double> periods;
    gen_periods(periods, 1.0, 0.05, 0.05);

    vector<string> directories;
    for(double period : periods){
        string directory = "period_" + to_string(period);
        cout << "Next directory: " << directory << endl; 
        directories.push_back(directory);
    }

    for(int i = 0; i < periods.size(); i++){
        filesystem::copy_options::overwrite_existing;
        filesystem::copy("load_test", directories[i]);
        filesystem::copy("dispatcher", directories[i]);
        filesystem::copy("sumo_latest.sif", directories[i]);
        cout << "Next period: " << periods[i] << endl;
        chdir(directories[i].c_str());
        string start = "./load_test " + num_partitions + " " + to_string(periods[i]) + " " + singularity_image;
        system(start.c_str());
        chdir("..");
    }

    return 0;
}
