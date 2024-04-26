#ifndef NETWORK_SWITCH_H
#define NETWORK_SWITCH_H

#include "../sketches/SketchBase.hpp"
#include <cstddef>

class NetworkSwitch {
public:
    // Constructor and destructor
    SketchBase* sketch;
    void* mem; // owns
    int memsize;
    size_t traffic;
    int group;

    NetworkSwitch(ControllerBase* controller, int memsize, int group);
    ~NetworkSwitch();

    // Public interface for processing packets
    void process(const void* packet, size_t count, size_t route_len, size_t cur);

};

#endif // NETWORK_SWITCH_H