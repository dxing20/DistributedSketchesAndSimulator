#include "NetworkSwitch.hpp"
#include <stdlib.h> 
#include <stdio.h>

NetworkSwitch::NetworkSwitch(ControllerBase* controller, int memsize, int group) {
    this->memsize = memsize;
    this->mem = malloc(memsize);
    this->traffic = 0;
    this->sketch = controller->newSketch(memsize, mem, group);
    this->group = group;
}

NetworkSwitch::~NetworkSwitch() {
    free(this->mem);
}

void NetworkSwitch::process(const void* packet, size_t count, size_t route_len, size_t cur){
    Packet p = Packet {
        .packet = packet,
        .count = count,
        .route_len = route_len,
        .cur = cur,
    };

    this->sketch->update(&p);
    
    this->traffic += count;
}
