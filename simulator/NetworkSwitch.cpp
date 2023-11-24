#include "NetworkSwitch.hpp"
#include <stdlib.h> 

NetworkSwitch::NetworkSwitch(ControllerBase* controller, int memsize) {
    this->memsize = memsize;
    this->mem = malloc(memsize);
    this->traffic = 0;
    // this->sketch = controller->newSketch(memsize, mem);
}

NetworkSwitch::~NetworkSwitch() {
    free(this->mem);
}

void NetworkSwitch::process(const void* packet, size_t count){
    // this->sketch->update(packet);
    
    this->traffic += count;
}
