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

    NetworkSwitch(ControllerBase* controller, int memsize);
    ~NetworkSwitch();

    // Public interface for processing packets
    void process(const void* packet, size_t count);

};

#endif // NETWORK_SWITCH_H