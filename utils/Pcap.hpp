#ifndef PCAP_HPP
#define PCAP_HPP

#define CHUNK 16384

void processPcapGz(const char* filename);
void processPcapFile(const char* filename);

#endif // PCAP_HPP