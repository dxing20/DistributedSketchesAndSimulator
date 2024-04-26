#include "BaseLineBloomFilter.hpp"
#include <cassert>
#include <cstdio>
#include <cstring>
#include "../utils/Seeds.hpp"
#include "../utils/SpookyV2.hpp"

#define SET_BIT(ptr, bitIdx) \
    (reinterpret_cast<uint8_t*>(ptr)[(bitIdx) / 8] |= (1 << ((bitIdx) % 8)))

#define CLEAR_BIT(ptr, bitIdx) \
    (reinterpret_cast<uint8_t*>(ptr)[(bitIdx) / 8] &= ~(1 << ((bitIdx) % 8)))

#define INSPECT_BIT(ptr, bitIdx) \
    ((reinterpret_cast<uint8_t*>(ptr)[(bitIdx) / 8] >> ((bitIdx) % 8)) & 1)



BLBF::BLBF(void* _args) {
    BLBFArgs* args = reinterpret_cast<BLBFArgs*>(_args);
    this->mem = args->mem;
    this->memsize = args->memsize;
    this->hashSeeds = std::vector<uint32_t>();

    uint32_t lastSeed = args->seed;
    int optimalHashCount = getOptimalHashFunctionCount(this->memsize);
    assert (optimalHashCount > 0);
    for(int i = 0; i < optimalHashCount; i++){
        this->hashSeeds.push_back(SpookyHash::Hash32(&lastSeed, 4, BLOOM_FILTER_HASH_CHAIN_HASH_SEED));
        lastSeed = this->hashSeeds.back();
    }
    std::memset(this->mem, 0, this->memsize);
}

BLBF::~BLBF() {
    // EMPTY
}

void* BLBF::initialize(void* args) {
    return NULL;
}

void* BLBF::update(const void* packet) {
    const void* flow = reinterpret_cast<const Packet*>(packet)->packet;
    size_t bitIdx;
    for(auto& seed: this->hashSeeds){
        bitIdx = SpookyHash::Hash32(flow, 13, seed)%(this->memsize*8);
        SET_BIT(this->mem, bitIdx);
    }
    return NULL;
}

void* BLBF::query(void* _args) {
    BLBFQuery* args = reinterpret_cast<BLBFQuery*>(_args);
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

void* BLBF::merge(void* args) {
    // TODO
    return NULL;
}

void* BLBF::construct(void* args) {
    // TODO
    return NULL;
}

void* BLBF::compress(void* args) {
    // TODO
    return NULL;
}

BLBFController::BLBFController() {
    this->sketch = std::vector<SketchBase*>();
}

BLBFController::~BLBFController() {
}

BLBF* BLBFController::newSketch(unsigned int memsize, void* mem, int group) {
    // TODO
    BLBFArgs args = {
        .mem = mem,
        .memsize=memsize,
        .seed=BLBF_HASH_SEED
    };
    BLBF* bf = new BLBF((void*)&args);
    this->sketch.push_back(bf);
    return bf;
}

void* BLBFController::query(void* args, std::vector<int>* route) {
    BLBFQuery* query = reinterpret_cast<BLBFQuery*>(args);
    for(auto& s: this->sketch){

        s->query(args);
        if(query->exists == 1){
            return NULL;
        }
    }
    return NULL;
}
