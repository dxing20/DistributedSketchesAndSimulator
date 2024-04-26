#include "Pcap.hpp"

#include <iostream>
#include <zlib.h>
#include <pcap.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <unordered_map>
#include <string>
#include <cstring>
#include "Config.hpp"

const std::unordered_map<u_char, std::string> protocolMap = {
    {IPPROTO_TCP, "TCP"},
    {IPPROTO_UDP, "UDP"},
    {IPPROTO_ICMP, "ICMP"},
    {IPPROTO_IGMP, "IGMP"},
    {IPPROTO_IP, "IP"},
    {IPPROTO_IPV6, "IPV6"},
    {IPPROTO_RAW, "RAW"},
    {IPPROTO_MAX, "MAX"},
};


u_char readIPTupleFromPacket(const u_char* packet, const struct pcap_pkthdr *header, u_char* tuple) {
    const struct ip* ipHeader = (struct ip*)(packet + sizeof(struct ether_header));
    uint headerSize = ipHeader->ip_hl * 4;
    u_short sourcePort = 0, destPort = 0;

    // Check if it's TCP or UDP to find ports (though for both its just first 4 byte)
    if (ipHeader->ip_p == IPPROTO_TCP) {
        const struct tcphdr* tcpHeader = (tcphdr*)(packet + sizeof(struct ether_header) + headerSize);
        sourcePort = ntohs(tcpHeader->th_sport);
        destPort = ntohs(tcpHeader->th_dport);
    } else if (ipHeader->ip_p == IPPROTO_UDP) {
        const struct udphdr* udpHeader = (udphdr*)(packet + sizeof(struct ether_header) + headerSize);
        sourcePort = ntohs(udpHeader->uh_sport);
        destPort = ntohs(udpHeader->uh_dport);
    } else {
        if (!PARSE_PORT){
            std::memcpy(tuple, &(ipHeader->ip_src), 4);
            std::memcpy(tuple + 4, &(ipHeader->ip_dst), 4);
            std::memcpy(tuple + 8, &sourcePort, 2);
            std::memcpy(tuple + 10, &destPort, 2);
            std::memcpy(tuple + 12, &ipHeader->ip_p, 1);
            return 1;
        }else{
            return 0;
        }
    }

    // Print the 5-tuple
    std::memcpy(tuple, &(ipHeader->ip_src), 4);
    std::memcpy(tuple + 4, &(ipHeader->ip_dst), 4);
    std::memcpy(tuple + 8, &sourcePort, 2);
    std::memcpy(tuple + 10, &destPort, 2);
    std::memcpy(tuple + 12, &ipHeader->ip_p, 1);
    return 1;
}

void processPcapFile(const char* filename) {
    char outfilename[256];
    char* outfilenameformat = "%s.bin";
    if (!PARSE_PORT){
        outfilenameformat = "%s_np.bin";
    }
    snprintf(outfilename, sizeof(outfilename), outfilenameformat, filename);
    FILE *outfile = fopen(outfilename, "wb");
    if (!outfile) {
        std::cerr << "Cannot create file: " << outfilename << std::endl;
        exit(EXIT_FAILURE);
    }

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *pcap = pcap_open_offline(filename, errbuf);
    if (pcap == NULL) {
        std::cerr << "Error opening pcap file: " << errbuf << std::endl;
        return;
    }

    const u_char *packet;
    struct pcap_pkthdr *header;
    int packetCount = 0;
    u_char* tuple = (u_char*) malloc(13);
    u_char success;

    while (pcap_next_ex(pcap, &header, &packet) >= 0) {
        // Process each packet here
        success = readIPTupleFromPacket(packet, header, tuple);
        if (success == 1) {
            fwrite(tuple, 1, 13, outfile);
        }
        packetCount++;
    }

    pcap_close(pcap);
    fclose(outfile);
}

void processPcapGz(const char* filename) {
    gzFile gzfile = gzopen(filename, "rb");
    if (!gzfile) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }

    char outfilename[256];
    snprintf(outfilename, sizeof(outfilename), "%s.pcap", filename);
    FILE *outfile = fopen(outfilename, "wb");
    if (!outfile) {
        std::cerr << "Cannot create file: " << outfilename << std::endl;
        gzclose(gzfile);
        exit(EXIT_FAILURE);
    }

    char buffer[CHUNK];
    int bytesRead;
    while ((bytesRead = gzread(gzfile, buffer, CHUNK)) > 0) {
        fwrite(buffer, 1, bytesRead, outfile);
    }

    fclose(outfile);
    gzclose(gzfile);

    processPcapFile(outfilename);
}