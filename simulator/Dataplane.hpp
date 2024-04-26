#ifndef TOPOLOGY_H
#define TOPOLOGY_H

#include <vector>
#include <unordered_map>
#include <list>
#include <cstring>
#include "../sketches/SketchBase.hpp"
#include "NetworkSwitch.hpp"
#include "../utils/SpookyV2.hpp"

class TopologyDefinition {
public:
    std::vector<std::vector<int>> adjMatrix;
    std::vector<int> memsizes;
    std::vector<int> edgeSwitchIds;
    std::unordered_map<int, int> switchToGroup;

    TopologyDefinition(std::vector<std::vector<int>> adjMatrix, std::vector<int> memsizes, std::vector<int> edgeSwitchIds, std::unordered_map<int, int> switchToGroup) {
        this->adjMatrix = adjMatrix;
        this->memsizes = memsizes;
        this->edgeSwitchIds = edgeSwitchIds;
        this->switchToGroup = switchToGroup;
    }
};

class Dataplane
{
public:
    // owns
    std::vector<NetworkSwitch*> switches; 
    std::vector<std::vector<std::vector<int>>> next;
    std::vector<int> edgeSwitchIds;

    ControllerBase *controller;

    Dataplane(ControllerBase *controller, const TopologyDefinition* topo);
    ~Dataplane();

    void runSim(const char* trace);
    void printForwardingTable();
    void queryController(void* arg, const void* flow);
    // size_t getRouteLen(const void* flow);

private:
    void getRoute(int srcSwitch, int dstSwitch, const void* flow, std::vector<int>* route);
    void flowToSwitchIdx(const void* flow, int* srcSwitch, int* dstSwitch);
    void floydWarshall(const TopologyDefinition* topo);
    void run_path(int srcSwitch, int dstSwitch, size_t count, const char* flow);
};

extern const TopologyDefinition FatTree20;

extern const TopologyDefinition FatTree7;


#endif // TOPOLOGY_H