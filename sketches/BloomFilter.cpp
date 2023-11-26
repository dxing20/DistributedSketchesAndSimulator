#include "BloomFilter.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include "../utils/SpookyV2.hpp"

#define SET_BIT(ptr, bitIdx) \
    (reinterpret_cast<uint8_t*>(ptr)[(bitIdx) / 8] |= (1 << ((bitIdx) % 8)))

#define CLEAR_BIT(ptr, bitIdx) \
    (reinterpret_cast<uint8_t*>(ptr)[(bitIdx) / 8] &= ~(1 << ((bitIdx) % 8)))

#define INSPECT_BIT(ptr, bitIdx) \
    ((reinterpret_cast<uint8_t*>(ptr)[(bitIdx) / 8] >> ((bitIdx) % 8)) & 1)



BloomFilter::BloomFilter(void* _args) {
    BloomFilterArgs* args = reinterpret_cast<BloomFilterArgs*>(_args);
    this->mem = args->mem;
    this->memsize = args->memsize;
    this->hashSeeds = std::vector<uint32_t>();

    uint32_t lastSeed = args->seed;
    int optimalHashCount = getOptimalHashFunctionCount(this->memsize);
    assert (optimalHashCount > 0);
    for(int i = 0; i < optimalHashCount; i++){
        this->hashSeeds.push_back(SpookyHash::Hash32(&lastSeed, 4, 1));
        lastSeed = this->hashSeeds.back();
    }
    std::memset(this->mem, 0, this->memsize);
}

BloomFilter::~BloomFilter() {
    // EMPTY
}

void* BloomFilter::initialize(void* args) {
    return NULL;
}

void* BloomFilter::update(const void* flow) {
    size_t bitIdx;
    for(auto& seed: this->hashSeeds){
        bitIdx = SpookyHash::Hash32(flow, 13, seed)%(this->memsize*8);
        SET_BIT(this->mem, bitIdx);
    }
    return NULL;
}

void* BloomFilter::query(void* _args) {
    BloomFilterQuery* args = reinterpret_cast<BloomFilterQuery*>(_args);
    size_t bitIdx;
    
    for(auto& seed: this->hashSeeds){
        bitIdx = SpookyHash::Hash32(args->flow, 13, seed)%(this->memsize*8);
        if(INSPECT_BIT(this->mem, bitIdx) != 1){
            args->exists = 0;
            return NULL;
        }
    }
    
    args->exists = 1;
    return NULL;
}

void* BloomFilter::merge(void* args) {
    // TODO
    return NULL;
}

void* BloomFilter::construct(void* args) {
    // TODO
    return NULL;
}

void* BloomFilter::compress(void* args) {
    // TODO
    return NULL;
}

BloomFilterController::BloomFilterController() {
    this->sketch = std::vector<SketchBase*>();
}

BloomFilterController::~BloomFilterController() {
}

BloomFilter* BloomFilterController::newSketch(unsigned int memsize, void* mem) {
    // TODO
    size_t nextIdx = this->sketch.size();
    BloomFilterArgs args = {
        .mem = mem,
        .memsize=memsize,
        .seed=nextIdx
    };
    BloomFilter* bf = new BloomFilter((void*)&args);
    this->sketch.push_back(bf);
    return bf;
}

void* BloomFilterController::query(void* args, std::vector<int>* route) {
    BloomFilterQuery* query = reinterpret_cast<BloomFilterQuery*>(args);
    for(auto& s: *route){

        this->sketch[s]->query(args);
        if(query->exists == 0){
            return NULL;
        }
    }
    return NULL;
}
