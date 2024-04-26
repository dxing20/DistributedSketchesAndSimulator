#include "frequency.hpp"
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <sstream>
#include <cstring>

std::string convertToIPAddress(const unsigned char bytes[4]) {
    std::stringstream ss;
    for (int i = 0; i < 4; ++i) {
        if (i != 0) {
            ss << ".";
        }
        ss << static_cast<int>(bytes[i]);
    }
    return ss.str();
}

std::string convertToFlow(const unsigned char bytes[13]) {
    std::stringstream ss;
    uint ip1, ip2;
    u_int16_t port1, port2;
    u_int8_t protocol;
    std::memcpy(&ip1, bytes, 4);
    std::memcpy(&ip2, bytes + 4, 4);
    std::memcpy(&port1, bytes + 8, 2);
    std::memcpy(&port2, bytes + 10, 2);
    std::memcpy(&protocol, bytes + 12, 1);

    ss << "(" << convertToIPAddress((unsigned char*)&ip1) << ":" << port1 << ", " << convertToIPAddress((unsigned char*)&ip2) << ":" << port2 << ", " << +protocol << ")";
    return ss.str();
}

void aggregateIP5Freq(const char* filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Cannot access file when aggregating IP5 frequency.\n" << filename << std::endl;
        exit(1);
    }

    std::unordered_map<std::string, int> freqMap;
    char* ipTuple = (char*) malloc(13);
    while (file.read(reinterpret_cast<char*>(ipTuple), 13)) {
        std::string ip5(ipTuple, 13);
        if (freqMap.find(ip5) == freqMap.end()) {
            freqMap[ip5] = 1;
        } else {
            freqMap[ip5] += 1;
        }
    }

    std::vector<std::pair<std::string, int>> pairs;
    for (auto& pair : freqMap) {
        pairs.push_back(pair);
    }

    std::sort(pairs.begin(), pairs.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        return a.second > b.second;
    });

    std::string outputFilename = std::string(filename) + ".txt";
    std::ofstream out(outputFilename.c_str());

    for (auto& pair : pairs) {
        std::string flowStr = convertToFlow((unsigned char*)pair.first.c_str());

        out << flowStr.c_str() << " x " << pair.second << std::endl;
    }

    out.close();
    free(ipTuple);
}

void aggregateRVFreq(const char* filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file) {
        std::cerr << "Cannot access file when aggregating IP5 frequency.\n";
        exit(1);
    }

    std::unordered_map<int, int> freqMap;
    int rv;
    while (file.read(reinterpret_cast<char*>(&rv), sizeof(rv))) {
        if (freqMap.find(rv) == freqMap.end()) {
            freqMap[rv] = 1;
        } else {
            freqMap[rv] += 1;
        }
    }

    std::vector<std::pair<int, int>> pairs;
    for (auto& pair : freqMap) {
        pairs.push_back(pair);
    }

    std::sort(pairs.begin(), pairs.end(), [](const std::pair<int, int>& a, const std::pair<int, int>& b) {
        return a.second > b.second;
    });

    std::string outputFilename = std::string(filename) + ".txt";
    std::ofstream out(outputFilename.c_str());

    for (auto& pair : pairs) {

        out << pair.first << " x " << pair.second << std::endl;
    }

    out.close();
}
