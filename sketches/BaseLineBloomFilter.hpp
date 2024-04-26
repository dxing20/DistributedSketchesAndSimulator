#ifndef BLBF_H
#define BLBF_H


#include "SketchBase.hpp"
#include <stdint.h>
#include <vector>
#include <cmath>
#include <cassert>

typedef struct BLBFArgs{
    void* mem;
    size_t memsize;
    size_t seed;
}BLBFArgs;

typedef struct BLBFQuery{
    const void* flow;
    unsigned char exists;
}BLBFQuery;

class BLBF : public SketchBase {
public:
    void* mem;
    int memsize;

    BLBF(void* args);
    ~BLBF();
    void* initialize(void* args);
    void* update(const void* args);
    void* query(void* args);
    void* merge(void* args);
    void* construct(void* args);
    void* compress(void* args);

private:
    static const size_t FLOWSIZE = 2000000;
    std::vector<uint32_t> hashSeeds;
    static int getOptimalHashFunctionCount(size_t memsize){
        assert(FLOWSIZE != 0);
        // Calculate the optimal number of hash functions
        double k = (memsize*8 / static_cast<double>(FLOWSIZE)) * log(2);

        return static_cast<int>(k+0.5); // round the integer value
    }
};

class BLBFController : public ControllerBase {
  public:
    BLBFController();
    ~BLBFController();
    BLBF* newSketch(unsigned int memsize, void* mem, int group);
    void* query(void* args, std::vector<int>* route);
};

#endif // BLBF