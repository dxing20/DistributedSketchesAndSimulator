#include "Evaluation.hpp"
#include <unordered_map>
#include "TraceReader.hpp"
#include "../sketches/BloomFilter.hpp"
#include "Frequency.hpp"
#include <time.h>
#include <stdlib.h>

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

    printf("All positive flow in sketch.\n");
    int falseNegatives = 0;
    int M = 100000;
    printf("Calculating false positives rates using %d random flow...\n", M);

    
    char* randomFlow = (char*) malloc(13);
    srand (time(NULL));

    for (int i = 0; i < M; i++) {
        do{
            generateRandomFlow(randomFlow);
        } while (freqMap.find(randomFlow) != freqMap.end());
        bfq.flow = randomFlow;
        dp->queryController(&bfq, bfq.flow);
        if (bfq.exists) {
            falseNegatives++;
        }
    }

    free(randomFlow);
    double falsePositiveRate = (double) falseNegatives / M;
    printf("False negatives: %d\n", falseNegatives);
    printf("False positive rate: %f\n", falsePositiveRate);

    printf("-- Evaluation: BloomFilter Complete --\n");
    return 0;
}