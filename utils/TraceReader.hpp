#ifndef TRACE_READER_H
#define TRACE_READER_H

#include <unordered_map>
#include <string>

void ip5FreqMap(const char* filename, std::unordered_map<std::string, int>* freqMap);


#endif // TRACE_READER_H