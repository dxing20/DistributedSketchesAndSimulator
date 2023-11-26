#include "simulator/Dataplane.hpp"
#include "utils/FiveTupleGen.hpp"
#include "utils/GenZipf.hpp"
#include "utils/Frequency.hpp"
#include "sketches/BloomFilter.hpp"
#include "utils/Evaluation.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <cstdio>

#ifdef DEBUG
#define DEBUG_FPRINTF(stream, format, ...) fprintf(stream, format, __VA_ARGS__)
#else
#define DEBUG_FPRINTF(stream, format, ...) 
#endif

float f1(void* real, void* est);
void* real_summary(const char* file_name);

std::string readNextStringArg(std::string* arg, std::string delimiter, const char* errOut)
{
    if (delimiter == "") {
        return *arg;
    }
    size_t pos = 0;
    std::string token;
    pos = arg->find(delimiter);
    if (pos == std::string::npos) {
        fprintf(stderr, errOut);
        exit(EXIT_FAILURE);
    }
    token = arg->substr(0, pos);
    arg->erase(0, pos + delimiter.length());
    return token;
}

const char* header = 
"=====================================================\n"
"   Distributed Sketch and Network Switch Simulator\n"
"=====================================================\n"
"   Usage: \n\t\t[-g \"<distribution>:<args>\"] \n\t\t[-s <trace_file>]\n"
"-----------------------------------------------------\n";

int main(int argc, char *argv[]) {
    int opt;
    std::string genzipfArg;
    u_int8_t gen;
    char* sim_trace;
    u_int8_t sim;

    fprintf(stdout, header);

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-g \"<distribution>:<args>\"] -s [trace_file]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    while ((opt = getopt(argc, argv, "g:s:")) != -1) {
        switch (opt) {
            case 'g':
                genzipfArg = std::string(optarg);
                gen = 1;
                break;
            case 's':
                sim = 1;
                sim_trace = optarg;
                break;
            default:
                fprintf(stderr, "Usage: %s [-g \"<distribution>:<args>\"] -s [trace_file]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (gen) {
        // Assuming some default values for the other genzipf parameters
        std::string distribution = readNextStringArg(&genzipfArg, ":", "Usage: [-g \"<distribution>:<args>\"] \n");

        if (distribution == "zipf") {
            // Assuming some default values for the other genzipf parameters
            int seed = atoi(readNextStringArg(&genzipfArg, ":", "Usage: [-g \"zipf:<seed>:<alpha>:<cardinality>:<count>\"] \n").c_str());
            float alpha = atof(readNextStringArg(&genzipfArg, ":", "Usage: [-g \"zipf:<seed>:<alpha>:<cardinality>:<count>\"] \n").c_str());
            double n = atof(readNextStringArg(&genzipfArg, ":", "Usage: [-g \"zipf:<seed>:<alpha>:<cardinality>:<count>\"] \n").c_str());
            int num_values = atoi(readNextStringArg(&genzipfArg, "", "Usage: [-g \"zipf:<seed>:<alpha>:<cardinality>:<count>\"] \n").c_str());
            std::string rvstreamfile = "trace/zipf_stream/zipf_" + std::to_string(seed) + "_" + std::to_string(alpha) + "_" + std::to_string(n) + "_" + std::to_string(num_values) + ".bin";
            std::string ip5file = "trace/ip5_zipf_" + std::to_string(seed) + "_" + std::to_string(alpha) + "_" + std::to_string(n) + "_" + std::to_string(num_values) + ".bin";
            genzipf(rvstreamfile.c_str(), seed, alpha, n, num_values);
            rvstream2ip5(rvstreamfile.c_str(), ip5file.c_str());
            aggregateIP5Freq(ip5file.c_str());
            aggregateRVFreq(rvstreamfile.c_str());
        } else {
            fprintf(stderr, "%s is not a recognized distribution \n", distribution.c_str());
            exit(EXIT_FAILURE);
        }
    }else if (sim){
        fprintf(stdout, "Initializing Simulator...\n");
        const TopologyDefinition* topo = &FatTree20;
        ControllerBase* controller = new BloomFilterController();
        Dataplane* dp = new Dataplane(controller, topo);

        fprintf(stdout, "Running simulation on trace: %s\n", sim_trace);

        dp->runSim(sim_trace);

        fprintf(stdout, "Simulation complete\n");
        
        BloomFilterEvaluation(sim_trace, dp);

        delete dp;

        delete controller;
    }

    return 0;
}