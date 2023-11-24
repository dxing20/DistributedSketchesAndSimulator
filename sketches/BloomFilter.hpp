#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H


#include "SketchBase.hpp"


class BloomFilter : public SketchBase {
  public:
    void* mem;
    int memsize;

    BloomFilter(void* args);
    ~BloomFilter();
    void* initialize(void* args);
    void* update(void* args);
    void* query(void* args);
    void* merge(void* args);
    void* construct(void* args);
    void* compress(void* args);
};

class BloomFilterController : public ControllerBase {
  public:
    BloomFilterController();
    ~BloomFilterController();
    BloomFilter* newSketch(unsigned int memsize, void* mem);
    void* query(void* args);
};

#endif // BLOOM_FILTER_H