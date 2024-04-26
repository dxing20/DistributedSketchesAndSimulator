#ifndef SKETCH_BASE_H
#define SKETCH_BASE_H

#include <vector>

typedef struct Packet {
    const void* packet;
    size_t count;
    size_t route_len;
    size_t cur;
} Packet;

class SketchBase {
  public:
    virtual ~SketchBase() {}

    virtual void* initialize(void* args) = 0;
    virtual void* update(const void* args) = 0;
    virtual void* query(void* args) = 0;
    virtual void* merge(void* args) = 0;
    virtual void* construct(void* args) = 0;
    virtual void* compress(void* args) = 0;
};

class ControllerBase {
  public:
    std::vector<SketchBase*> sketch; // owns

    virtual ~ControllerBase() {
        for (auto s : sketch) {
            delete s;
        }
    
    }

    virtual SketchBase* newSketch(unsigned int memsize, void* mem, int group) = 0;
    virtual void* query(void* args, std::vector<int>* route ) = 0;
};

#endif // SKETCH_BASE_H