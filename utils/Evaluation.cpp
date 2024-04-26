#include "Evaluation.hpp"
#include <unordered_map>
#include "TraceReader.hpp"
#include "../sketches/BloomFilter.hpp"
#include "../sketches/KMVSuperSpreader.hpp"
#include "Frequency.hpp"
#include <time.h>
#include <stdlib.h>
#include "Seeds.hpp"
#include <unordered_set>

void generateRandomFlow(char* flow) {
    for (int i = 0; i < 13; i++) {
        flow[i] = rand() % 256;
    }
}

double BloomFilterEvaluation(const char* trace, Dataplane* dp) {
    printf("-- Evaluation: BloomFilter --\n");
    printf("Reading trace file...\n");
    std::unordered_map<std::string, int> freqMap;
    ip5FreqMap(trace, &freqMap);

    BloomFilterQuery bfq = BloomFilterQuery();

    printf("Verifying all positives...\n");

    for (auto& pair : freqMap) {
        bfq.flow = pair.first.c_str();
        dp->queryController(&bfq, bfq.flow); 
        if (!bfq.exists) {
            fprintf(stderr, "Flow %s from trace does not exist in the BloomFilter\n", convertToFlow(reinterpret_cast<const unsigned char*>(bfq.flow)).c_str());
            exit(EXIT_FAILURE);
        }
    }

    printf("No False Negatives in sketch.\n");
    int falsePositive = 0;
    int M = 1000000;
    printf("Calculating false positives rates using %d random flow...\n", M);

    
    char* randomFlow = (char*) malloc(13);
    srand (RANDOM_FLOW_FOR_BLOOM_FILTER_EVALUATION_HASH_SEED);

    for (int i = 0; i < M; i++) {
        do{
            generateRandomFlow(randomFlow);
        } while (freqMap.find(randomFlow) != freqMap.end());
        bfq.flow = randomFlow;
        dp->queryController(&bfq, bfq.flow);
        if (bfq.exists) {
            falsePositive++;
        }
    }

    free(randomFlow);
    double falsePositiveRate = (double) falsePositive / M;
    printf("False positives: %d\n", falsePositive);
    printf("False positive rate: %f\n", falsePositiveRate);

    printf("-- Evaluation: BloomFilter Complete --\n");
    return 0;
}

std::pair<double, double> KMVSuperSpreaderEvaluation(const char* trace, Dataplane* dp){
    printf("-- Evaluation: KMVSuperSpreader --\n");
    printf("Reading trace file...\n");
    std::unordered_map<std::string, int> freqMap;
    ip5FreqMap(trace, &freqMap);

    printf("Querying...\n");

    KMVSuperSpreaderQuery* query = new KMVSuperSpreaderQuery();
    query->fileout = "superspreaders.csv";

    dp->controller->query(query, NULL);

    printf("Finished querying KMVSuperSpreader\n");
    
    // std::unordered_map<int, std::unordered_map<std::string, std::unordered_set<unsigned int>>> *superspreadersByGroup = reinterpret_cast<std::unordered_map<int, std::unordered_map<std::string, std::unordered_set<unsigned int>>> *>(dp->controller->query(query, NULL));

    std::unordered_map<std::string, std::unordered_set<std::string>> trueSrcFreq;

    char src[6];
    char dest[6];
    int c = 0;

    for (auto& pair : freqMap){
        memcpy(src, pair.first.c_str(), 4);
        memcpy(src + 4, pair.first.c_str() + 8, 2);
        memcpy(dest, pair.first.c_str() + 4, 4);
        memcpy(dest + 4, pair.first.c_str() + 10, 2);

        std::string strsrc (src, 6);
        std::string strdst (dest, 6);

        if (trueSrcFreq.find(strsrc) == trueSrcFreq.end()){
            trueSrcFreq.insert({strsrc, std::unordered_set<std::string>()});
        }
        trueSrcFreq[strsrc].insert(strdst);
    }

    // write to csv

    FILE* csv = fopen("superspreaders_true.csv", "w");
    fprintf(csv, "src,freq\n");
    for (auto& p : trueSrcFreq){
        unsigned int freq = p.second.size();
        const unsigned char* src = reinterpret_cast<const unsigned char*>(p.first.c_str());
        unsigned short port;
        memcpy(&port, p.first.c_str() + 4, 2);
        fprintf(csv, "%s:%d,%u\n", convertToIPAddress(src).c_str(), port, freq);
    }

    fclose(csv);

    

    printf("-- Evaluation: KMVSuperSpreader Complete --\n");

    exit(0);

    
    return std::make_pair(0, 0);

    
}
