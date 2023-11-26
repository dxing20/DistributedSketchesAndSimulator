#include "Dataplane.hpp"
#include <cassert>
#include "../utils/TraceReader.hpp"
#include <climits>
#include <algorithm>
#include "../utils/debug_util.hpp"
#include "../utils/Frequency.hpp"
#include "../utils/Seeds.hpp"

const TopologyDefinition FatTree20 = TopologyDefinition(
    std::vector<std::vector<int>> {
        {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
        {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
        {0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
        {0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
        {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0}, 
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0}, 
        {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0}, 
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0}, 
        {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0}, 
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0}, 
        {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}, 
        {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1}, 
        {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
        {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
        {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0}, 
    },
        std::vector<int> {
        4096, 4096, 4096, 4096, 1024,
        1024, 1024, 1024, 1024, 1024,
        1024, 1024, 1024, 1024, 1024,
        1024, 1024, 1024, 1024, 1024,
    },
        std::vector<int> {
        12, 13, 14, 15, 16, 17, 18, 19
    }
);

const TopologyDefinition FatTree7 = TopologyDefinition(
    std::vector<std::vector<int>> {
        {0, 1, 1, 0, 0, 0, 0}, 
        {1, 0, 0, 1, 1, 0, 0}, 
        {1, 0, 0, 0, 0, 1, 1}, 
        {0, 1, 0, 0, 0, 0, 0}, 
        {0, 1, 0, 0, 0, 0, 0}, 
        {0, 0, 1, 0, 0, 0, 0}, 
        {0, 0, 1, 0, 0, 0, 0}, 
    },
    std::vector<int> {
        1024, 1024, 1024, 1024, 1024, 1024, 1024,
    },
    std::vector<int> {
        3, 4, 5, 6
    }
);

Dataplane::Dataplane(ControllerBase *controller, const TopologyDefinition* topo) {
    this->controller = controller;
    this->edgeSwitchIds = topo->edgeSwitchIds;
    for (size_t i = 0; i < topo->adjMatrix.size(); i++) {
        this->switches.push_back(new NetworkSwitch(controller, topo->memsizes[i]));
    }
    this->floydWarshall(topo);
    
}

Dataplane::~Dataplane() {
    for (size_t i = 0; i < this->switches.size(); i++) {
        delete this->switches[i];
    }
}

void Dataplane::floydWarshall(const TopologyDefinition* topo) {
    int V = topo->adjMatrix.size();
    int dist[V][V];

    for (int i = 0; i < V; i++) {
        this->next.push_back(std::vector<std::vector<int>>(V));
    }

    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (i == j){
                dist[i][j] = 0;
                continue;
            }
            if (topo->adjMatrix[i][j] == 1) {
                dist[i][j] = 1;
                next[i][j].push_back(j);
            }else{
                dist[i][j] = INT_MAX;
            }
        }
    }

    for (int k = 0; k < V; k++) {
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                if (dist[i][k] == INT_MAX || dist[k][j] == INT_MAX) {
                    continue;
                }
                if (dist[i][k] + dist[k][j] < dist[i][j]) {
                    dist[i][j] = dist[i][k] + dist[k][j];
                    next[i][j].clear();
                    for(auto& n : next[i][k]) {
                        next[i][j].push_back(n);
                    }
                }else if (dist[i][k] + dist[k][j] == dist[i][j]) {
                    for(auto& n : next[i][k]) {
                        if (std::find(next[i][j].begin(), next[i][j].end(), n) == next[i][j].end())
                            next[i][j].push_back(n);
                    }
                }
            }
        }
    }

    DEBUG_EXEC(DebugType::FORWARDING, {
        this->printForwardingTable();
    });
}

void Dataplane::printForwardingTable() {
    int V = this->switches.size();
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++){
            DEBUG_PRINTF(DebugType::FORWARDING, "optimal next hops for %d to %d: \n", i+1, j+1);
            for (auto& n : next[i][j]) {
                DEBUG_PRINTF(DebugType::FORWARDING, "\t%d\n", n+1);
            }
        }
        DEBUG_PRINT(DebugType::FORWARDING, "\n");
    }
}

void Dataplane::runSim(const char* trace) {
    fprintf(stdout, "Reading trace file...\n");
    std::unordered_map<std::string, int> freqMap;
    ip5FreqMap(trace, &freqMap);
    fprintf(stdout, "Simulating flows...\n");
    int32_t srcSwitch, dstSwitch;
    size_t edgeSwitchCount = this->edgeSwitchIds.size();
    assert(edgeSwitchCount > 0);

    for (auto& pair : freqMap) {
        const char* temp5 = pair.first.c_str();
        int freq = pair.second;
        // indices of edgeswitchidsv
        flowToSwitchIdx(temp5, &srcSwitch, &dstSwitch);
        this->run_path(srcSwitch, dstSwitch, freq, temp5);
    }
}

void Dataplane::run_path(int srcSwitchId, int dstSwitchId, size_t count, const char* flow){
    /*
    * srcSwitchId: index of the source switch
    * dstSwitchId: index of the destination switch
    * count: number of packets to simulate
    * flow: 13 bytes (src: 4 bytes, dst: 4 bytes, src port: 2 bytes, dst port: 2 bytes, protocol: 1 byte)
    * 
    * Given a pair of source and destination switch indices, simulate count packets from src to dst using
    * ECMP on flow
    */

    DEBUG_EXEC(DebugType::ROUTING, {
        std::string flowStr = convertToFlow((unsigned char*)flow);
        DEBUG_PRINTF(DebugType::ROUTING, ">> %s : srcSwitchId: %d, dstSwitchId: %d, count: %ld\n", flowStr.c_str(), srcSwitchId+1, dstSwitchId+1, count);
    });

    std::vector<int> route = std::vector<int>();
    this->getRoute(srcSwitchId, dstSwitchId, flow, &route);

    // Uncomment this to not simulate hypervisor on same node, bypassing sketch
    // if (route.size() == 1){
    //     return;
    // }

    for (size_t i = 0; i < route.size(); i++) {
        DEBUG_PRINTF(DebugType::ROUTING, "  hop %ld: %d(idx)\n", i+1, route[i]+1);
        this->switches[route[i]]->process(flow, count);
    }
}

void Dataplane::queryController(void* arg, const void* flow) {
    /*
    * arg: argument to pass to the controller
    * flow: 13 bytes (src: 4 bytes, dst: 4 bytes, src port: 2 bytes, dst port: 2 bytes, protocol: 1 byte)
    *
    * Given a flow, query the controller with the route of the packet.
    */
    int srcSwitch, dstSwitch;
    this->flowToSwitchIdx(flow, &srcSwitch, &dstSwitch);
    std::vector<int> route = std::vector<int>();
    this->getRoute(srcSwitch, dstSwitch, flow, &route);
    this->controller->query(arg, &route);
}

void Dataplane::flowToSwitchIdx(const void* flow, int* srcSwitch, int* dstSwitch){
    /*
    * flow: 13 bytes (src: 4 bytes, dst: 4 bytes, src port: 2 bytes, dst port: 2 bytes, protocol: 1 byte)
    * src: 4 bytes
    * dst: 4 bytes
    * 
    * Maps src and dst ips to a pair of switch index
    */
    uint32_t srcIP;
    uint32_t dstIP;
    std::memcpy(&srcIP, (unsigned char*)flow, 4);
    std::memcpy(&dstIP, (unsigned char*)flow + 4, 4);
    *srcSwitch = this->edgeSwitchIds[SpookyHash::Hash32(&srcIP, 4, IP_TO_EDGESWITCH_IDX_HASH_SEED) % this->edgeSwitchIds.size()];
    *dstSwitch = this->edgeSwitchIds[SpookyHash::Hash32(&dstIP, 4, IP_TO_EDGESWITCH_IDX_HASH_SEED) % this->edgeSwitchIds.size()];
}

void Dataplane::getRoute(int srcSwitch, int dstSwitch, const void* flow, std::vector<int>* route){
    /*
    * srcSwitch: index of the source switch
    * dstSwitch: index of the destination switch
    * flow: 13 bytes (src: 4 bytes, dst: 4 bytes, src port: 2 bytes, dst port: 2 bytes, protocol: 1 byte)
    * route: a vector of switch indices
    * 
    * Given a pair of source and destination switch indices, get the route from src to dst using
    * ECMP on flow
    */
    int cur = srcSwitch;
    route->push_back(cur);
    if (cur == dstSwitch){
        return;
    }
    do {
        if (this->next[cur][dstSwitch].size() == 0){
            fprintf(stderr, "Failed to find hops for flow: %d -> %d, cur=%d", srcSwitch+1, dstSwitch+1, cur+1);
            exit(EXIT_FAILURE);
        }else if (this->next[cur][dstSwitch].size() == 1){
            cur = this->next[cur][dstSwitch][0];
        }else{
            uint32_t seed = ECMP_NEXT_HOP_HASH_SEED;
            int nextIdx = SpookyHash::Hash32(flow, 13, seed) % this->next[cur][dstSwitch].size();
            cur = this->next[cur][dstSwitch][nextIdx];
        }
        route->push_back(cur);
    }while(cur != dstSwitch);
}
