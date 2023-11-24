#ifndef FREQUENCY_H
#define FREQUENCY_H

#include <string>

std::string convertToIPAddress(const unsigned char bytes[4]);
std::string convertToFlow(const unsigned char bytes[13]);
void aggregateIP5Freq(const char* filename);
void aggregateRVFreq(const char* filename);

#endif // FREQUENCY_H
