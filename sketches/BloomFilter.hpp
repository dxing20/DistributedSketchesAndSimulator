#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H


#include "SketchBase.hpp"
#include <stdint.h>
#include <vector>
#include <cmath>
#include <cassert>

typedef struct BloomFilterArgs{
    void* mem;
    size_t memsize;
    size_t seed;
}BloomFilterArgs;

typedef struct BloomFilterQuery{
    const void* flow;
    unsigned char exists;
}BloomFilterQuery;

class BloomFilter : public SketchBase {
public:
    void* mem;
    int memsize;

    BloomFilter(void* args);
    ~BloomFilter();
    void* initialize(void* args);
    void* update(const void* args);
    void* query(void* args);
    void* merge(void* args);
    void* construct(void* args);
    void* compress(void* args);

private:
    static const size_t FLOWSIZE = 3000;
    std::vector<uint32_t> hashSeeds;
    static int getOptimalHashFunctionCount(size_t memsize){
        assert(FLOWSIZE != 0);
        // Calculate the optimal number of hash functions
        double k = (memsize*8 / static_cast<double>(FLOWSIZE)) * log(2);

        return static_cast<int>(k+0.5); // round the integer value
    }
};

class BloomFilterController : public ControllerBase {
  public:
    BloomFilterController();
    ~BloomFilterController();
    BloomFilter* newSketch(unsigned int memsize, void* mem);
    void* query(void* args, std::vector<int>* route);
};

#endif // BLOOM_FILTER_H