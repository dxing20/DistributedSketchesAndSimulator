#include "TraceReader.hpp"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>


void ip5FreqMap(const char* filename, std::unordered_map<std::string, int>* freqMap)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Cannot access file when aggregating IP5 frequency.\n";
        return;
    }

    char* ipTuple = (char*) malloc(13);
    while (file.read(reinterpret_cast<char*>(ipTuple), 13)) {
        std::string ip5(ipTuple, 12);
        if (freqMap->find(ip5) == freqMap->end()) {
            (*freqMap)[ip5] = 1;
        } else {
            (*freqMap)[ip5] += 1;
        }
    }
    free(ipTuple);
}