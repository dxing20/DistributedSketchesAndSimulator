#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>
#include "SpookyV2.hpp"

void longHash(int value, char* dst);
void rv2ip5(int rv, char* ipTuple);

void rvstream2ip5(const std::string& src, const std::string& dst) {
    std::ifstream srcFile(src, std::ios::binary);
    std::ofstream dstFile(dst, std::ios::binary);

    if (!srcFile || !dstFile) {
        std::cerr << "Cannot access file when converting random value to 5-tuple ip.\n";
        return;
    }

    int value;
    char* ipTuple = (char*) malloc(13); // src ip, dst ip, src port, dst port, protocol
    while (srcFile.read(reinterpret_cast<char*>(&value), sizeof(value))) {
        // Convert value to 5-tuple ip
        rv2ip5(value, ipTuple);
        // Write to dstFile
        dstFile.write(reinterpret_cast<char*>(ipTuple), 13);
    }

    free(ipTuple);
    srcFile.close();
    dstFile.close();
}

void rv2ip5(int rv, char* ipTuple) {
    // hash rv, get the first 12 bytes, add 0x00 to the end
    longHash(rv, ipTuple);
    std::memcpy(ipTuple + 12, "\x00", 1);
}


void longHash(int value, char* dst) {
    // produces a 12 byte hash by using 
    uint64 hash1 = 1, hash2 = 2;
    SpookyHash::Hash128(&value, sizeof(value), &hash1, &hash2);

    // Copy the first 12 bytes of hash1 and hash2 to dst
    std::memcpy(dst, &hash1, 8);
    std::memcpy(dst + 8, &hash2, 4);
}