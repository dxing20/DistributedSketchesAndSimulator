#include "Dataplane.hpp"
#include <cassert>
#include "../utils/TraceReader.hpp"
#include <climits>
#include "../utils/debug_util.hpp"
#include "../utils/Frequency.hpp"

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
    uint32 seed = 1;
    unsigned char* src = (unsigned char*) malloc(6);
    unsigned char* dst = (unsigned char*) malloc(6);
    unsigned char* prot = (unsigned char*) malloc(1);
    int32_t edgeSwitchSrc, edgeSwitchDst;
    size_t edgeSwitchCount = this->edgeSwitchIds.size();
    assert(edgeSwitchCount > 0);

    for (auto& pair : freqMap) {
        const char* temp5 = pair.first.c_str();
        std::memcpy(src, temp5, 6);
        std::memcpy(dst, temp5 + 6, 6);
        std::memcpy(prot, temp5 + 12, 1);
        int freq = pair.second;

        // indices of edgeswitchids
        edgeSwitchSrc = SpookyHash::Hash32(src, 4, seed) % edgeSwitchCount;
        edgeSwitchDst = SpookyHash::Hash32(dst, 4, seed) % edgeSwitchCount;

        this->run_path(this->edgeSwitchIds[edgeSwitchSrc], this->edgeSwitchIds[edgeSwitchDst], freq, temp5);
    }

    free(src);
    free(dst);
}

void Dataplane::run_path(int srcSwitchId, int dstSwitchId, size_t count, const char* flow){
    // the number of possible next hops
    DEBUG_EXEC(DebugType::ROUTING, {
       std::string flowStr = convertToFlow((unsigned char*)flow);
        DEBUG_PRINTF(DebugType::ROUTING, ">> %s : srcSwitchId: %d, dstSwitchId: %d, count: %ld\n", flowStr.c_str(), srcSwitchId+1, dstSwitchId+1, count);
    });
    size_t possibleNextNum;
    int cur = srcSwitchId;
    int nextIdx;
    uint32 seed = 1;

    // hypervisor no switch
    // if (srcSwitchId == dstSwitchId){
    //     return;
    // }

    this->switches[cur]->process(flow, count);

    while(cur != dstSwitchId){
        possibleNextNum = this->next[cur][dstSwitchId].size();
        DEBUG_PRINTF(DebugType::ROUTING, "cur: %d, dst: %d, number of available options: %ld\n", cur+1, dstSwitchId+1, possibleNextNum);
        if (possibleNextNum == 0){
            fprintf(stderr, "Failed to find hops for flow: %d -> %d, cur=%d", srcSwitchId+1, dstSwitchId+1, cur+1);
            exit(EXIT_FAILURE);
        }else if (possibleNextNum == 1){
            DEBUG_PRINTF(DebugType::ROUTING, "\tselected only option: %d\n", this->next[cur][dstSwitchId][0]+1);
            cur = this->next[cur][dstSwitchId][0];
        }else{
            nextIdx = SpookyHash::Hash32(flow, 13, seed) % possibleNextNum;
            DEBUG_PRINTF(DebugType::ROUTING, "\tselected: %d\n", this->next[cur][dstSwitchId][nextIdx]+1);
            cur = this->next[cur][dstSwitchId][nextIdx];
        }
        this->switches[cur]->process(flow, count);
    };
}